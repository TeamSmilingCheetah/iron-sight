#include "pch.h"
#include "Engine/System/Public/Manager/CollisionManager.h"

#include "Engine/Runtime/Public/Component/Physics/ColliderBase.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

/**************************/
/** Broad Phase Function **/
/**************************/

/**
 * @brief 모든 Dynamic Collider를 특정 BVH와 충돌 검사를 진행하여 세부 판정이 필요한 충돌쌍을 확인하는 함수
 *
 * @param InTreeRootNode AABB 충돌을 검증할 VBH 트리의 Root Node
 * @param InCandidateCheckSet 중복 충돌쌍을 배제하기 위한 Broad Phase 검증 Set
 */
void FCollisionManager::GetCandidatesInBVH(const BVHNode* InTreeRootNode,
                                               unordered_set<FCollisionID>& InCandidateCheckSet)
{
	for (IColliderBase* LeftCollider : DynamicColliders)
	{
		// BVH에 대한 쿼리를 실행하여 AABB가 겹치는 충돌 후보 선별
		vector<IColliderBase*> AABBCollidedCandidates;
		QueryBVH(InTreeRootNode, LeftCollider, AABBCollidedCandidates);

		// AABB가 겹치는 충돌체에 대해서 검증한 뒤, 세부 충돌 확인이 필요한 경우 AddCandidate
		for (IColliderBase* RightCollider : AABBCollidedCandidates)
		{
			if (IsInCondition(InCandidateCheckSet, LeftCollider, RightCollider))
			{
				AddCandidate(LeftCollider, RightCollider);
			}
		}
	}
}

/**
 * @brief 각 충돌체가 포함된 Layer의 충돌 여부를 확인하는 함수
 *
 * @param InLeftCollider Collider 1
 * @param InRightCollider Collider 2
 * @return 충돌체가 포함된 Layer 간 충돌 여부
 */
bool FCollisionManager::IsLayerCollided(const IColliderBase* InLeftCollider, const IColliderBase* InRightCollider) const
{
	// Get Layer
	int LeftObjectIndex = InLeftCollider->GetOwner()->GetLayerIdx();
	int RightObjectIndex = InRightCollider->GetOwner()->GetLayerIdx();

	if (LeftObjectIndex > RightObjectIndex)
	{
		std::swap(LeftObjectIndex, RightObjectIndex);
	}

	if (LayerCollisionMatrix[LeftObjectIndex] & (1 << RightObjectIndex))
	{
		return true;
	}

	return false;
}

/**
 * @brief 충돌 가능성이 보이는 충돌체 쌍을 후보군 벡터에 올리는 함수
 *
 * @param InLeftCollider Collider 1
 * @param InRightCollider Collider 2
 */
void FCollisionManager::AddCandidate(IColliderBase* InLeftCollider, IColliderBase* InRightCollider)
{
	CollisionCandidates.push_back(FCollisionID(InLeftCollider, InRightCollider));
}
