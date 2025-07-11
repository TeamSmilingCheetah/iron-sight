#pragma once
#include "Engine/Runtime/Public/Component/Physics/CMeshCollider.h"
#include "Engine/System/Public/Rendering/Shader/CMeshCollisionCS.h"

union COLLIDER_ID
{
	struct
	{
		UINT Left;
		UINT Right;
	};

	ULONGLONG ID;
};

class CCollider2D;
class CCollider3D;
class CColliderRay;
class CLandScape;
class CMeshCollider;

/**
 * @brief 충돌을 관리하는 매니저 클래스
 *
 * @param m_Matrix 각 레이어 간의 충돌 검사 여부를 저장해둔 Matrix
 * @param m_ColInfo 충돌한 오브젝트들의 ID와 충돌 상태를 저장해둔 Map
 * @param m_RayColInfo Raycast 충돌 검사를 위한 Ray Colliders
 * @param m_LandObject Landscape 충돌 처리를 위한 Landscape Objects
 */
class CCollisionMgr :
	public singleton<CCollisionMgr>
{
	SINGLE(CCollisionMgr);

private:
	UINT m_Matrix[MAX_LAYER];
	map<ULONGLONG, bool> m_ColInfo;
	set<CColliderRay*> m_RayColInfo;
	set<CLandScape*> m_LandObject;
	CMeshCollisionCS m_MeshCollisionCS;

public:
	struct SimpleVtx
	{
		Vec3 pos;
	};
	struct SimpleIdx
	{
		uint32_t x, y, z;
	};

private:
	// Layer Collision Main Logic
	void CollisionsInLayer(UINT PLayerIndex);
	void ProcessCollisionMatrix();
	void CollisionCheckWithTypeMatching(const CGameObject* PLeftObject, const CGameObject* PRightObject);

	// Collision By Type
	void CollisionBtwLayer(UINT PLeftIndex, UINT PRightIndex);
	void CollisionBtwCollider2D(CCollider2D* PLeftCol, CCollider2D* PRightCol);
	void CollisionBtwCollider3D(CCollider3D* PLeftCol, CCollider3D* PRightCol);
	void CollisionBtwLandScape3D(CCollider3D* PLeftCol, CLandScape* PRightCol);
	void CollisionBtwColliderRay(CColliderRay* PLeftCol, CCollider3D* PRightCol);
	void CollisionBtwLandScapeRay(CColliderRay* PLeftCol, CLandScape* PRightCol);

	template <typename T>
	void CollisionWithMesh(CMeshCollider* PLeftCollider, T* PRightCollider);

	// Collision Determination
	bool IsCollision(CCollider2D* PLeft, CCollider2D* PRight);
	bool IsCollision3D(CCollider3D* PLeft, CCollider3D* PRight);
	bool IsCollisionRay(CColliderRay* PLeftCol, CCollider3D* PRightCol);
	bool IsCollision3DLand(CCollider3D* PLeftCol, CLandScape* PRightCol);
	bool IsCollisionRayLand(CColliderRay* PLeftCol, CLandScape* PRightCol);

	bool IsBoundingBoxCollided(CMeshCollider* PLeftCollider, CMeshCollider* PRightCollider);
	bool IsBoundingBoxCollided(CMeshCollider* PMeshCollider, const CCollider3D* P3DCollider);
	bool IsMeshCollided(CMeshCollider* PLeftCollider, CMeshCollider* PRightCollider);
	bool IsMeshCollided(CMeshCollider* PMeshCollider, CCollider3D* P3DCollider);

	// Landscape & Ray
	void LandCheck();
	void RayOverlapCheck();

public:
	void Tick();
	void ToggleLayerCollision(UINT PLeft, UINT PRight);
	void CollisionCheckClear() { memset(m_Matrix, 0, sizeof(UINT) * MAX_LAYER); }
};

/**
 * @brief Mesh와 Mesh끼리 충돌했을 경우에 충돌 처리하는 케이스에 대한 함수
 *
 * 1. 바운딩 박스 체크해서 서로 닿는 범위에 있는지 확인
 * 2. 바운딩 박스가 겹친다면 정점 전부 검사
 *
 * @param PLeftCollider Collider 1
 * @param PRightCollider Collider 2
 */
template <typename T>
void CCollisionMgr::CollisionWithMesh(CMeshCollider* PLeftCollider, T* PRightCollider)
{
	COLLIDER_ID id;
	id.Left = PLeftCollider->GetID();
	id.Right = PRightCollider->GetID();

	// 기존 충돌 정보와 대조 후 없으면 추가
	auto iter = m_ColInfo.find(id.ID);
	if (iter == m_ColInfo.end())
	{
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	// Bounding Box Check & Mech Collision Check
	// 단락 평가로 Bounding Box 통과 시에만 Mesh 충돌 검사 진행
	if (IsBoundingBoxCollided(PLeftCollider, PRightCollider) && IsMeshCollided(PLeftCollider, PRightCollider))
	{
		bool IsDead = PLeftCollider->GetOwner()->IsDead() || PRightCollider->GetOwner()->IsDead();
		bool IsLayerChanged = PLeftCollider->GetOwner()->IsLayerMove() || PRightCollider->GetOwner()->IsLayerMove();

		if (iter->second)
		{
			if (IsDead || IsLayerChanged)
			{
				PLeftCollider->EndOverlap(PRightCollider);
				PRightCollider->EndOverlap(PLeftCollider);
				iter->second = false;
			}
			else
			{
				PLeftCollider->Overlap(PRightCollider);
				PRightCollider->Overlap(PLeftCollider);
			}
		}
		else
		{
			if (!IsDead)
			{
				PLeftCollider->BeginOverlap(PRightCollider);
				PRightCollider->BeginOverlap(PLeftCollider);
				iter->second = true;
			}
		}
	}
	else
	{
		if (iter->second)
		{
			PLeftCollider->EndOverlap(PRightCollider);
			PRightCollider->EndOverlap(PLeftCollider);
			iter->second = false;
		}
	}
}
