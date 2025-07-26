#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"

void CollisionManager::CollisionPostProcess()
{
	// Layer Matching
	for (UINT Row = 0; Row < MAX_LAYER; ++Row)
	{
		for (UINT Column = Row; Column < MAX_LAYER; ++Column)
		{
			if (LayerCollisionMatrix[Row] & (1 << Column))
			{
				// 1. 단일 레이어 내의 충돌 처리
				if (Row == Column)
				{
					CollisionsInLayer(Row);
				}
				// 2. Matching된 Layer 간의 발생하는 충돌 처리
				else
				{
					CollisionBtwLayer(Row, Column);
				}
			}
		}
	}
}

/**
 * @brief 단일 레이어 내의 오브젝트 간의 충돌에 대해 처리하는 함수
 *
 * @param InLayerIndex 레벨 내의 충돌 검사할 Object 정보들이 들어 있는 Layer Index
 */
void CollisionManager::CollisionsInLayer(UINT InLayerIndex)
{
	const vector<CGameObject*>& ObjectVector = CLevelMgr::GetInst()->GetCurrentLevel()
	                                                               ->GetLayer(InLayerIndex)->GetObjects();

	// 동일 레이어 내에서는 중복을 고려하여 절반의 횟수만 처리하면 된다
	for (size_t i = 0; i < ObjectVector.size(); ++i)
	{
		for (size_t j = i + 1; j < ObjectVector.size(); ++j)
		{
			for (auto LeftCollider : ObjectVector[i]->GetColliders())
			{
				for (auto RightCollider : ObjectVector[j]->GetColliders())
				{
					ExecuteOverlap(LeftCollider, RightCollider);
				}
			}
		}
	}
}

/**
 * @brief 두 레이어 간의 충돌 처리 함수
 *
 * @param InLeftLayerIndex 레벨 내의 충돌 검사할 Object 정보들이 들어 있는 Layer Index 1
 * @param InRightLayerIndex 레벨 내의 충돌 검사할 Object 정보들이 들어 있는 Layer Index 2
 */
void CollisionManager::CollisionBtwLayer(UINT InLeftLayerIndex, UINT InRightLayerIndex)
{
	const vector<CGameObject*>& LeftObjectVector = CLevelMgr::GetInst()->GetCurrentLevel()
	                                                                   ->GetLayer(InLeftLayerIndex)->GetObjects();
	const vector<CGameObject*>& RightObjectVector = CLevelMgr::GetInst()->GetCurrentLevel()
	                                                                    ->GetLayer(InRightLayerIndex)->GetObjects();

	// 서로 다른 레이어의 경우 모든 충돌 검사를 위해서는 sizeL * sizeR 전부 확인해야 한다
	for (size_t i = 0; i < LeftObjectVector.size(); ++i)
	{
		for (size_t j = 0; j < RightObjectVector.size(); ++j)
		{
			for (auto LeftCollider : LeftObjectVector[i]->GetColliders())
			{
				for (auto RightCollider : RightObjectVector[j]->GetColliders())
				{
					ExecuteOverlap(LeftCollider, RightCollider);
				}
			}
		}
	}
}

/**
 * @brief 모든 충돌 로직이 완료된 상황에 Object 간의 Overlap에 대한 처리를 일괄적으로 진행하는 함수
 *
 * @param InLeftCollider Collider 1
 * @param InRightCollider Collider 2
 */
void CollisionManager::ExecuteOverlap(ColliderVariant InLeftCollider, ColliderVariant InRightCollider)
{
	visit([&](auto* LeftCollider, auto* RightCollider)
	{
		COLLISION_ID CollisionID(LeftCollider->GetID(), RightCollider->GetID());

		auto iter = ColllisionMap.find(CollisionID.ID);
		if (iter == ColllisionMap.end())
		{
			ColllisionMap.insert({CollisionID.ID, false});
			iter = ColllisionMap.find(CollisionID.ID);
		}

		CGameObject* LeftObject = LeftCollider->GetOwner();
		CGameObject* RightObject = RightCollider->GetOwner();

		bool IsDead = LeftObject->IsDead() || RightObject->IsDead();
		bool IsLayerChanged = LeftObject->IsLayerMove() || RightObject->IsLayerMove();
		bool IsDeactive = LeftObject->IsDeactivated() || RightObject->IsDeactivated()
			|| LeftCollider->GetState() == DEACTIVE || RightCollider->GetState() == DEACTIVE;

		if (FrameCollisionSet.contains(CollisionID.ID))
		{
			if (iter->second)
			{
				// EndOverlap
				if (IsDead || IsLayerChanged || IsDeactive)
				{
					LeftCollider->EndOverlap(RightCollider);
					RightCollider->EndOverlap(LeftCollider);
					iter->second = false;
				}
				// Overlap
				else
				{
					LeftCollider->Overlap(RightCollider);
					RightCollider->Overlap(LeftCollider);
				}
			}
			else
			{
				// BeginOverlap
				if (!IsDead && !IsDeactive)
				{
					LeftCollider->BeginOverlap(RightCollider);
					RightCollider->BeginOverlap(LeftCollider);
					iter->second = true;
				}
			}
		}
		else
		{
			// EndOverlap
			if (iter->second)
			{
				LeftCollider->EndOverlap(RightCollider);
				RightCollider->EndOverlap(LeftCollider);
				iter->second = false;
			}
		}
	}, InLeftCollider, InRightCollider);
}
