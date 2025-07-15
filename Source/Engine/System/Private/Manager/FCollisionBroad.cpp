#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Runtime/Public/Component/Physics/CCollider2D.h"
#include "Runtime/Public/Component/Physics/CCollider3D.h"
#include "Runtime/Public/Component/Physics/CColliderRay.h"
#include "Runtime/Public/Component/Physics/CMeshCollider.h"
#include "Runtime/Public/Component/Rendering/CLandScape.h"

/*****************/
/** Broad Check **/
/*****************/

/**
 * @brief Layer 간의 충돌에 대해서 처리하는 함수
 */
void FCollisionManager::CheckBroadPhase()
{
	// Layer Matching
	for (UINT Row = 0; Row < MAX_LAYER; ++Row)
	{
		for (UINT Col = Row; Col < MAX_LAYER; ++Col)
		{
			if (MLayerCollisionMatrix[Row] & (1 << Col))
			{
				// 1. 단일 레이어 내의 Broad 충돌 판정
				if (Row == Col)
				{
					CollisionsInLayer(Row);
				}
				// 2. Matching된 Layer 간의 발생하는 Broad 충돌 판정
				else
				{
					CollisionBtwLayer(Row, Col);
				}
			}
		}
	}
}

/**
 * @brief 단일 레이어 내의 오브젝트 간의 Broad 충돌 판정 함수
 *
 * @param PLayerIndex 레벨 내의 Broad 충돌 판정할 Object 정보들이 들어 있는 Layer Index
 */
void FCollisionManager::CollisionsInLayer(UINT PLayerIndex)
{
	const vector<CGameObject*>& ObjectVector = CLevelMgr::GetInst()->GetCurrentLevel()
	                                                               ->GetLayer(PLayerIndex)->GetObjects();

	// 동일 레이어 내에서는 중복을 고려하여 절반의 횟수만 처리하면 된다
	for (size_t i = 0; i < ObjectVector.size(); ++i)
	{
		for (size_t j = i + 1; j < ObjectVector.size(); ++j)
		{
			CheckPairWide(ObjectVector[i], ObjectVector[j]);
		}
	}
}

/**
 * @brief 두 레이어 간의 Broad 충돌 판정 함수
 *
 * @param PLeftIndex 레벨 내의 Broad 충돌 판정할 Object 정보들이 들어 있는 Layer Index 1
 * @param PRightIndex 레벨 내의 Broad 충돌 판정할 Object 정보들이 들어 있는 Layer Index 2
 */
void FCollisionManager::CollisionBtwLayer(UINT PLeftIndex, UINT PRightIndex)
{
	const vector<CGameObject*>& LeftObjectVector = CLevelMgr::GetInst()->GetCurrentLevel()
	                                                                   ->GetLayer(PLeftIndex)->GetObjects();
	const vector<CGameObject*>& RightObjectVector = CLevelMgr::GetInst()->GetCurrentLevel()
	                                                                    ->GetLayer(PRightIndex)->GetObjects();

	// 서로 다른 레이어의 경우 모든 충돌 검사를 위해서는 sizeL * sizeR 전부 확인해야 한다
	for (size_t i = 0; i < LeftObjectVector.size(); ++i)
	{
		for (size_t j = 0; j < RightObjectVector.size(); ++j)
		{
			CheckPairWide(LeftObjectVector[i], RightObjectVector[j]);
		}
	}
}

/******************************/
/** Redirect & Add Candidate **/
/******************************/

/**
 * @brief 해당 오브젝트의 충돌 컴포넌트 타입에 맞춰 적절하게 Broad 충돌 판정을 진행하는 함수
 *
 * @param PLeftObject Object 1
 * @param PRightObject Object 2
 */
void FCollisionManager::CheckPairWide(CGameObject* PLeftObject, CGameObject* PRightObject)
{
	// 1. 2D 충돌체 검사
	if (PLeftObject->Collider2D())
	{
		if (PRightObject->Collider2D())
		{
			if (CheckAABB(PLeftObject->Collider2D(), PRightObject->Collider2D()))
			{
				AddCandidate(PLeftObject, PRightObject);
			}
			return;
		}
	}

	// 2. 3D 충돌체 검사
	if (PLeftObject->Collider3D())
	{
		if (PRightObject->Collider3D())
		{
			if (CheckAABB(PLeftObject->Collider3D(), PRightObject->Collider3D()))
			{
				AddCandidate(PLeftObject, PRightObject);
			}
			return;
		}

		if (PRightObject->LandScape())
		{
			if (CheckAABB(PLeftObject->Collider3D(), PRightObject->LandScape()))
			{
				AddCandidate(PLeftObject, PRightObject);
			}
			return;
		}

		if (PRightObject->ColliderRay())
		{
			if (CheckAABB(PRightObject->ColliderRay(), PLeftObject->Collider3D()))
			{
				AddCandidate(PLeftObject, PRightObject);
			}
			return;
		}

		if (PRightObject->MeshCollider())
		{
			if (CheckAABB(PRightObject->MeshCollider(), PLeftObject->Collider3D()))
			{
				AddCandidate(PLeftObject, PRightObject);
			}
			return;
		}
	}

	// 3. LandScape 검사
	if (PLeftObject->LandScape())
	{
		// 3D간 충돌의 경우
		if (PRightObject->Collider3D())
		{
			if (CheckAABB(PRightObject->Collider3D(), PLeftObject->LandScape()))
			{
				AddCandidate(PLeftObject, PRightObject);
			}
			return;
		}

		// Ray와 충돌의 경우
		if (PRightObject->ColliderRay())
		{
			if (CheckAABB(PRightObject->ColliderRay(), PLeftObject->LandScape()))
			{
				AddCandidate(PLeftObject, PRightObject);
			}
			return;
		}
	}

	// 4. RayCast 검사
	if (PLeftObject->ColliderRay())
	{
		if (PRightObject->Collider3D())
		{
			if (CheckAABB(PLeftObject->ColliderRay(), PRightObject->Collider3D()))
			{
				AddCandidate(PLeftObject, PRightObject);
			}
			return;
		}

		// LandScape와 충돌의 경우
		if (PRightObject->LandScape())
		{
			if (CheckAABB(PLeftObject->ColliderRay(), PRightObject->LandScape()))
			{
				AddCandidate(PLeftObject, PRightObject);
			}
			return;
		}
	}

	// 5. Mesh Collider
	if (PLeftObject->MeshCollider())
	{
		if (PRightObject->MeshCollider())
		{
			if (CheckAABB(PLeftObject->MeshCollider(), PRightObject->MeshCollider()))
			{
				AddCandidate(PLeftObject, PRightObject);
			}
			return;
		}

		if (PRightObject->Collider3D())
		{
			if (CheckAABB(PLeftObject->MeshCollider(), PRightObject->Collider3D()))
			{
				AddCandidate(PLeftObject, PRightObject);
			}
			return;
		}
	}
}

/**
 * @brief 충돌 가능성이 보이는 오브젝트 쌍을 후보군 벡터에 올리는 함수
 *
 * @param PLeftObject Object 1
 * @param PRightObject Object 2
 */
void FCollisionManager::AddCandidate(CGameObject* PLeftObject, CGameObject* PRightObject)
{
	MCandidatePairVector.emplace_back(PLeftObject, PRightObject);
}

/*********************************/
/** Broad Collision Check Logic **/
/*********************************/

/**
 * @brief 2D 충돌체끼리 Broad Collision 체크를 진행하는 함수
 *
 * @param PLeftCollider 3D Collider 1
 * @param PRightCollider 3D Collider 2
 * @return 충돌 여부
 */
bool FCollisionManager::CheckAABB(const CCollider2D* PLeftCollider, const CCollider2D* PRightCollider)
{
	// Get 2D AABB
	Vec2 LeftMin, LeftMax, RightMin, RightMax;
	PLeftCollider->GetAABB(LeftMin, LeftMax);
	PRightCollider->GetAABB(RightMin, RightMax);

	// AABB Collision Test
	if (LeftMax.x < RightMin.x || LeftMin.x > RightMax.x)
	{
		return false;
	}
	if (LeftMax.y < RightMin.y || LeftMin.y > RightMax.y)
	{
		return false;
	}

	return true;
}

/**
 * @brief 3D 충돌체끼리 Broad Collision 체크를 진행하는 함수
 *
 * @param PLeftCollider 3D Collider 1
 * @param PRightCollider 3D Collider 2
 * @return 충돌 여부
 */
bool FCollisionManager::CheckAABB(const CCollider3D* PLeftCollider, const CCollider3D* PRightCollider)
{
	// Get 3D Collider AABB
	const auto& LeftBox = PLeftCollider->GetColliderAABB();
	const auto& RightBox = PRightCollider->GetColliderAABB();
	Vec3 LeftMin = LeftBox.Min;
	Vec3 LeftMax = LeftBox.Max;
	Vec3 RightMin = RightBox.Min;
	Vec3 RightMax = RightBox.Max;

	// AABB Collision Test
	if (LeftMax.x < RightMin.x || LeftMin.x > RightMax.x)
	{
		return false;
	}
	if (LeftMax.y < RightMin.y || LeftMin.y > RightMax.y)
	{
		return false;
	}
	if (LeftMax.z < RightMin.z || LeftMin.z > RightMax.z)
	{
		return false;
	}

	return true;
}

/**
 * @brief 각 메시 충돌체의 바운딩 박스끼리 충돌했는지 여부를 확인하는 함수
 *
 * @param PLeftCollider Mesh Collider 1
 * @param PRightCollider Mesh Collider 2
 * @return 충돌 여부
 */
bool FCollisionManager::CheckAABB(const CMeshCollider* PLeftCollider, const CMeshCollider* PRightCollider)
{
	// Get AABB
	const auto& LeftBox = PLeftCollider->GetOwner()->GetAABB();
	const auto& RightBox = PRightCollider->GetOwner()->GetAABB();
	Vec3 LeftMin = LeftBox.Min;
	Vec3 LeftMax = LeftBox.Max;
	Vec3 RightMin = RightBox.Min;
	Vec3 RightMax = RightBox.Max;

	// AABB Collision Test
	if (LeftMax.x < RightMin.x || LeftMin.x > RightMax.x)
	{
		return false;
	}
	if (LeftMax.y < RightMin.y || LeftMin.y > RightMax.y)
	{
		return false;
	}
	if (LeftMax.z < RightMin.z || LeftMin.z > RightMax.z)
	{
		return false;
	}

	return true;
}

/**
 * @brief 메시 충돌체의 바운딩 박스와 충돌체가 충돌했는지 여부를 확인하는 함수
 *
 * @param PLeftCollider Mesh Collider
 * @param PRightCollider 3D Collider
 * @return 충돌 여부
 */
bool FCollisionManager::CheckAABB(const CMeshCollider* PLeftCollider, const CCollider3D* PRightCollider)
{
	// Get AABB
	const auto& MeshBox = PLeftCollider->GetOwner()->GetAABB();
	const auto& ColliderBox = PRightCollider->GetColliderAABB();
	Vec3 MeshMin = MeshBox.Min;
	Vec3 MeshMax = MeshBox.Max;
	Vec3 ColliderMin = ColliderBox.Min;
	Vec3 ColliderMax = ColliderBox.Max;

	// AABB Collision Test
	if (MeshMax.x < ColliderMin.x || MeshMin.x > ColliderMax.x)
	{
		return false;
	}
	if (MeshMax.y < ColliderMin.y || MeshMin.y > ColliderMax.y)
	{
		return false;
	}
	if (MeshMax.z < ColliderMin.z || MeshMin.z > ColliderMax.z)
	{
		return false;
	}

	return true;
}

/**
 * @brief 3D 충돌체 AABB와 Landscape가 충돌하는지 확인하는 함수
 *
 * @param PLeftCollider 3D Collider
 * @param PRightCollider Landscape Collider
 * @return 충돌 여부
 */
bool FCollisionManager::CheckAABB(const CCollider3D* PLeftCollider, const CLandScape* PRightCollider)
{
	// Get AABB
	const auto& LeftBox = PLeftCollider->GetColliderAABB();
	const auto& RightBox = PRightCollider->GetAABB();
	Vec3 LeftMin = LeftBox.Min;
	Vec3 LeftMax = LeftBox.Max;
	Vec3 RightMin = RightBox.Min;
	Vec3 RightMax = RightBox.Max;

	// AABB Collision Test
	if (LeftMax.x < RightMin.x || LeftMin.x > RightMax.x)
	{
		return false;
	}
	if (LeftMax.y < RightMin.y || LeftMin.y > RightMax.y)
	{
		return false;
	}
	if (LeftMax.z < RightMin.z || LeftMin.z > RightMax.z)
	{
		return false;
	}

	return true;
}

/**
 * @brief 3D 충돌체 AABB와 Ray가 충돌하는지 확인하는 함수
 *
 * @param PLeftCollider Ray Collider
 * @param PRightCollider 3D Collider
 * @return 충돌 여부
 */
bool FCollisionManager::CheckAABB(const CColliderRay* PLeftCollider, const CCollider3D* PRightCollider)
{
	// Get Ray Information
	Vec3 RayStart = PLeftCollider->GetRayFinalPos();
	Vec3 RayDirection = PLeftCollider->GetRayFinalDir();
	float RayMaxDistance = PLeftCollider->GetRayLength();

	// Early Return
	if (RayDirection.Length() < 1e-6f)
	{
		return false;
	}

	// Get Collider AABB
	const auto& ColliderBox = PRightCollider->GetColliderAABB();
	Vec3 BoxMin = ColliderBox.Min;
	Vec3 BoxMax = ColliderBox.Max;

	// DirectX Intersect
	BoundingBox AABB;
	AABB.Center = (BoxMax + BoxMin) / 2.f;
	AABB.Extents = (BoxMax - BoxMin) / 2.f;
	float IntersectionDistance;

	if (AABB.Intersects(RayStart, RayDirection, IntersectionDistance))
	{
		if (IntersectionDistance <= RayMaxDistance)
		{
			return true;
		}
	}

	return false;
}

/**
 * @brief Landscape AABB와 Ray가 충돌하는지 확인하는 함수
 *
 * @param PLeftCollider Ray Collider
 * @param PRightCollider Landscape Collider
 * @return 충돌 여부
 */
bool FCollisionManager::CheckAABB(const CColliderRay* PLeftCollider, const CLandScape* PRightCollider)
{
	// Get Ray Information
	Vec3 RayStart = PLeftCollider->GetRayFinalPos();
	Vec3 RayDirection = PLeftCollider->GetRayFinalDir();
	float RayLength = PLeftCollider->GetRayLength();

	// Find Ray End
	Vec3 RayEnd = RayStart + RayDirection * RayLength;

	// Ray AABB
	Vec3 RayMin = Vec3(
		std::min(RayStart.x, RayEnd.x),
		std::min(RayStart.y, RayEnd.y),
		std::min(RayStart.z, RayEnd.z)
	);
	Vec3 RayMax = Vec3(
		std::max(RayStart.x, RayEnd.x),
		std::max(RayStart.y, RayEnd.y),
		std::max(RayStart.z, RayEnd.z)
	);

	// LandScape AABB
	const auto& LandscapeAABB = PRightCollider->GetAABB();
	Vec3 LandscapeMin = LandscapeAABB.Min;
	Vec3 LandscapeMax = PRightCollider->GetAABB().Max;

	// AABB Collision Test
	if (RayMax.x < LandscapeMin.x || RayMin.x > LandscapeMax.x)
	{
		return false;
	}
	if (RayMax.y < LandscapeMin.y || RayMin.y > LandscapeMax.y)
	{
		return false;
	}
	if (RayMax.z < LandscapeMin.z || RayMin.z > LandscapeMax.z)
	{
		return false;
	}

	return true;
}
