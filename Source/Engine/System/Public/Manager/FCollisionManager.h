#pragma once
#include "Runtime/Public/Component/Physics/CMeshCollider.h"
#include "Runtime/Public/Component/Physics/CCollider3D.h"
#include "Runtime/Public/Component/Physics/CColliderRay.h"
#include "Runtime/Public/Component/Rendering/CLandScape.h"
#include "System/Public/Rendering/Shader/CMeshCollisionCS.h"
#include "System/Public/Rendering/Shader/CRaycastCS.h"

union COLLISION_ID
{
	struct
	{
		UINT Left;
		UINT Right;
	};

	ULONGLONG ID;

	COLLISION_ID(UINT PLeft, UINT PRight)
	{
		Left = PLeft;
		Right = PRight;
	}
};

/**
 * @brief 충돌을 관리하는 매니저 클래스
 *
 * @param MLayerCollisionMatrix 각 레이어 간의 충돌 검사 여부를 저장해둔 Matrix
 * @param MColllisionInfoMap 충돌한 오브젝트들의 ID와 충돌 상태를 저장해둔 Map
 * @param MMeshCollisionCS Mesh 충돌 판정 처리를 위한 Compute Shader
 * @param MCandidatePairVector Narrow 판정 처리를 위한 가능성 Pair를 모아두는 Vector
 */
class FCollisionManager :
	public singleton<FCollisionManager>
{
	SINGLE(FCollisionManager);

private:
	UINT MLayerCollisionMatrix[MAX_LAYER];
	map<ULONGLONG, bool> MColllisionInfoMap;
	CMeshCollisionCS MMeshCollisionCS;
        CRaycastCS MRaycastCS;
	vector<pair<const CGameObject*, const CGameObject*>> MCandidatePairVector;
	BVHNode* MBVHRootNode = nullptr;

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
	// Main Flow
	void CreateBVHTree();
	void RaycastProcess();
	void BroadPhase();
	void NarrowPhase();
	void CleanResource();

	// BVH Function
	void BuildBVH(const vector<CGameObject*>& PObjects);
	void DestroyBVH();
	static BVHNode* BuildBVHRecursive(const vector<CGameObject*>& PObjects, int PDepth = 0);

	// Raycast Function
	static void QueryBVH(const BVHNode* PNode, const CColliderRay* PRay, vector<RayColliderInfo>& PIntersectVector);
	static bool IsIntersect(const CColliderRay* PLeftCollider, const CMeshCollider* PRightCollider);

	// Broad Phase Function
	static void QueryBVH(const BVHNode* PNode, const CGameObject* PObject, vector<CGameObject*>& PCandidates);
	void AddCandidate(const CGameObject* PLeftObject, const CGameObject* PRightObject);

	// Narrow Phase Function
	void CheckPair(const CGameObject* PRightObject, const CGameObject* PLeftObject);
	static bool IsCollision(const CCollider2D* PLeftCollider, const CCollider2D* PRightCollider);
	static bool IsCollision(const CCollider3D* PLeftCollider, const CCollider3D* PRightCollider);
	static bool IsCollision(const CCollider3D* PLeftCollider, const CLandScape* PRightCollider);
	bool IsCollision(CMeshCollider* PLeftCollider, CMeshCollider* PRightCollider);
	bool IsCollision(CMeshCollider* PLeftCollider, CCollider3D* PRightCollider);
	bool IsCollision(CCollider3D* PLeftCollider, CMeshCollider* PRightCollider);

	// Collision Apply
	template <typename T1, typename T2>
	void ProcessCollision(T1* PLeftCollider, T2* PRightCollider);

public:
	void Tick();
	void ActiveLayerCollision(UINT PLeft, UINT PRight);
	void ClearCollisionBtwLayerSetting();
};

/**
 * @brief 충돌체 간의 충돌 상태에 따라 오브젝트의 이후 동작을 유도하는 함수
 *
 * @tparam T1 Collider Type 1
 * @tparam T2 Collider Type 2
 * @param PLeftCollider Collider 1
 * @param PRightCollider Collider 2
 */
template <typename T1, typename T2>
void FCollisionManager::ProcessCollision(T1* PLeftCollider, T2* PRightCollider)
{
	// Set Collision ID
	COLLISION_ID CollisionID(PLeftCollider->GetID(), PRightCollider->GetID());

	// 기존 충돌 정보와 대조 후 Map에 없으면 추가
	auto CollisionPair = MColllisionInfoMap.find(CollisionID.ID);
	if (CollisionPair == MColllisionInfoMap.end())
	{
		MColllisionInfoMap.insert(make_pair(CollisionID.ID, false));
		CollisionPair = MColllisionInfoMap.find(CollisionID.ID);
	}

	CGameObject* LeftObject = PLeftCollider->GetOwner();
	CGameObject* RightObject = PRightCollider->GetOwner();

	bool IsDead = LeftObject->IsDead() || RightObject->IsDead();
	bool IsDeactive = !LeftObject->IsActive() || !RightObject->IsActive();
	bool IsLayerChanged = LeftObject->IsLayerMove() || RightObject->IsLayerMove();

	if (CollisionPair->second)
	{
		// EndOverlap
		if (IsDead || IsDeactive || IsLayerChanged)
		{
			PLeftCollider->EndOverlap(PRightCollider);
			PRightCollider->EndOverlap(PLeftCollider);
			CollisionPair->second = false;
		}
		// Overlap
		else
		{
			PLeftCollider->Overlap(PRightCollider);
			PRightCollider->Overlap(PLeftCollider);
		}
	}
	else
	{
		// BeginOverlap
		if (!IsDead && !IsDeactive)
		{
			PLeftCollider->BeginOverlap(PRightCollider);
			PRightCollider->BeginOverlap(PLeftCollider);
			CollisionPair->second = true;
		}
	}
}
