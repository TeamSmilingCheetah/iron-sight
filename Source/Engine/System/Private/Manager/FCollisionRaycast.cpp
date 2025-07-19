#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Physics/CMeshCollider.h"

/*******************/
/** Raycast Check **/
/*******************/

/**
 * @brief 레벨 내 모든 Ray Collider와 충돌 가능한 오브젝트에 대해 Raycast를 처리하는 함수
 */
void FCollisionManager::RaycastProcess()
{
	CLevel* CurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	vector<CGameObject*> AllObjects;
	CurrentLevel->GetAllActiveObjectsInLevel(AllObjects);

	// 모든 Ray Collider 수집
	vector<CColliderRay*> RayColliders;
	for (const auto* Object : AllObjects)
	{
		if (Object->ColliderRay())
		{
			RayColliders.push_back(Object->ColliderRay());
		}
	}

	// LOG_INFO_F("[Collision][Raycast] Total Active Raycast Objects: {}", RayColliders.size());

	// Check Collision
	for (auto* Ray : RayColliders)
	{
		// Find All Candidates
		vector<RayColliderInfo> Candidates;
		QueryBVH(MBVHRootNode, Ray, Candidates);

		for (const RayColliderInfo& Info : Candidates)
		{
			auto* Object = Info.HitObject;

			if (Object->Collider3D())
			{
				// AABB가 충돌 기준이므로 바로 Process
				ProcessCollision(Ray, Object->Collider3D());
				continue;
			}
			if (Object->LandScape())
			{
				// AABB가 충돌 기준이므로 바로 Process
				ProcessCollision(Ray, Object->LandScape());
				continue;
			}
			if (Object->MeshCollider())
			{
				if (IsIntersect(Ray, Object->MeshCollider()))
				{
					ProcessCollision(Ray, Object->MeshCollider());
					continue;
				}
			}
		}
	}
}

bool FCollisionManager::IsIntersect(const CColliderRay* PLeftCollider, const CMeshCollider* PRightCollider)
{
	// TODO(KHJ): TBD
	return true;
}

