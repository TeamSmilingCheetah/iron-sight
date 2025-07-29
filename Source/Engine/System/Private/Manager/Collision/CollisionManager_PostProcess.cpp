#include "pch.h"
#include "Engine/System/Public/Manager/CollisionManager.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Physics/ColliderBase.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"

/**
 * @brief 모든 충돌 판정이 완료된 이후 Object 간의 Overlap에 대한 처리를 일괄적으로 진행하는 함수
 */
void FCollisionManager::ExecuteOverlap() const
{
	// LOG_TRACE_F("[Collision][ExecuteOverlap] Total Collision In This Frame: {}", FrameCollisionSet->size());

	for (const FCollisionID Collision : *FrameCollisionSet)
	{
		IColliderBase* LeftCollider = Collision.Left;
		IColliderBase* RightCollider = Collision.Right;
		const CGameObject* LeftObject = LeftCollider->GetOwner();
		const CGameObject* RightObject = RightCollider->GetOwner();

		bool IsDead = LeftObject->IsDead() || RightObject->IsDead();
		bool IsLayerChanged = LeftObject->IsLayerMove() || RightObject->IsLayerMove();
		bool IsDeactive = LeftObject->IsDeactivated() || RightObject->IsDeactivated()
			|| LeftCollider->IsDeactive() || RightCollider->IsDeactive();

		if (PrevFrameCollisionSet->contains(Collision))
		{
			// LOG_TRACE_F("[Collision][Overlap] {} & {} EndOverlapped",
			//             WStringToString(LeftObject->GetName()), WStringToString(RightObject->GetName()));

			// EndOverlap
			if (IsDead || IsLayerChanged || IsDeactive)
			{
				LeftCollider->DecreaseOverlapCount();
				RightCollider->DecreaseOverlapCount();

				for (auto* Script : LeftObject->GetScripts())
				{
					Script->EndOverlap(LeftCollider, RightCollider);
				}
				for (auto* Script : RightObject->GetScripts())
				{
					Script->EndOverlap(RightCollider, LeftCollider);
				}

				// 여기서 EndOverlap하기 때문에 다음 프레임에 잡히지 않도록 제거
				FrameCollisionSet->erase(Collision);
			}
			// Overlap
			else
			{
				// LOG_TRACE_F("[Collision][Overlap] {} & {} Overlapped",
				//             WStringToString(LeftObject->GetName()), WStringToString(RightObject->GetName()));

				for (auto* Script : LeftObject->GetScripts())
				{
					Script->Overlap(LeftCollider, RightCollider);
				}
				for (auto* Script : RightObject->GetScripts())
				{
					Script->Overlap(RightCollider, LeftCollider);
				}
			}

			// EndOverlap에 중복으로 처리되지 않도록 여기서 제거
			PrevFrameCollisionSet->erase(Collision);
		}
		else
		{
			// BeginOverlap
			if (!IsDead && !IsDeactive)
			{
				// LOG_TRACE_F("[Collision][Overlap] {} & {} BeginOverlapped",
				//             WStringToString(LeftObject->GetName()), WStringToString(RightObject->GetName()));

				LeftCollider->IncreaseOverlapCount();
				RightCollider->IncreaseOverlapCount();

				for (auto* Script : LeftObject->GetScripts())
				{
					Script->BeginOverlap(LeftCollider, RightCollider);
				}
				for (auto* Script : RightObject->GetScripts())
				{
					Script->BeginOverlap(RightCollider, LeftCollider);
				}
			}
		}
	}

	// 위에서 제거되지 않은 Previous Collision에 대해서는 전부 EndOverlap 처리
	for (const FCollisionID PrevCollision : *PrevFrameCollisionSet)
	{
		IColliderBase* LeftCollider = PrevCollision.Left;
		IColliderBase* RightCollider = PrevCollision.Right;
		const CGameObject* LeftObject = LeftCollider->GetOwner();
		const CGameObject* RightObject = RightCollider->GetOwner();

		// LOG_TRACE_F("[Collision][Overlap] {} & {} EndOverlapped",
		//             WStringToString(LeftObject->GetName()), WStringToString(RightObject->GetName()));

		LeftCollider->DecreaseOverlapCount();
		RightCollider->DecreaseOverlapCount();

		for (auto* Script : LeftObject->GetScripts())
		{
			Script->EndOverlap(LeftCollider, RightCollider);
		}
		for (auto* Script : RightObject->GetScripts())
		{
			Script->EndOverlap(RightCollider, LeftCollider);
		}
	}
}
