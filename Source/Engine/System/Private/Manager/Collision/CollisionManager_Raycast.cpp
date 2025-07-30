#include "pch.h"
#include "Engine/System/Public/Manager/CollisionManager.h"

#include "Engine/System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "Engine/Runtime/Public/Component/Physics/Collider3D.h"
#include "Engine/Runtime/Public/Component/Physics/ColliderRay.h"
#include "Engine/Runtime/Public/Component/Physics/MeshCollider.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

/*******************/
/** Raycast Check **/
/*******************/

/**
 * @brief BVH를 순회하며 교차하는 AABB를 후보군에 등록하는 함수
 */
void FCollisionManager::RaycastBroad()
{
	// Check All Collision
	for (FColliderRay* Ray : RayColliders)
	{
		QueryBVH(StaticBVHRoot, Ray, RayCandidates);
		QueryBVH(DynamicBVHRoot, Ray, RayCandidates);
	}
}

/**
 * @brief 후보군을 전부 탐색하면서 충돌이 확인된 경우 Frame Collision 대상으로 올려두는 함수
 */
void FCollisionManager::RaycastNarrow()
{
	for (const FRayCollisionInfo& Info : RayCandidates)
	{
		FColliderRay* Ray = Info.RayObject;
		IColliderBase* Collider = Info.HitCollider;

		unordered_set<FCollisionID> CheckSet;
		if (IsInCondition(CheckSet, Ray, Collider))
		{
			// AABB가 충돌 기준이므로 바로 Process
			if (Collider->GetColliderType() == EColliderType::Collider3D)
			{
				AddFrameCollision(Info.RayObject, static_cast<FCollider3D*>(Collider));
				continue;
			}

			// RaycastCS 처리를 위한 Add Task
			if (Collider->GetColliderType() == EColliderType::MeshCollider)
			{
				AddRayShaderTask(Ray, static_cast<FMeshCollider*>(Collider));
			}
		}
	}

	// Process Delayed Batch Process
	ExecuteAndProcessRaycastCS();
}

/**
 * @brief 충돌체와 실제로 충돌하는지 확인하기 위한 CS Task에 추가하는 함수
 *
 * @param InRay Ray Collider
 * @param InCollider Other Collider (Mesh)
 */
void FCollisionManager::AddRayShaderTask(FColliderRay* InRay, const FMeshCollider* InCollider)
{
	FMeshBatchData MeshData = GetOrAddBatchData(InCollider);
	RaycastTaskColliders.push_back(make_pair(InRay, InCollider->MeshCollider()));

	tRaycastTask Task = {};
	Task.RayOrigin = InRay->GetRayFinalPos();
	Task.RayDirection = InRay->GetRayFinalDir();
	Task.VertexOffset = MeshData.VertexOffset;
	Task.IndexOffset = MeshData.IndexOffset;
	Task.TriCount = MeshData.TriangleCount;

	RaycastTasks.push_back(Task);
}

/**
 * @brief Raycast Batch Process를 실행하고 결과를 처리하는 함수
 */
void FCollisionManager::ExecuteAndProcessRaycastCS()
{
	// Early Return
	if (RaycastTasks.empty())
	{
		return;
	}

	// 1. GPU에 보낼 구조화된 버퍼 생성
	CStructuredBuffer AllVtxBuffer, AllIdxBuffer, TasksBuffer, ResultsBuffer;
	AllVtxBuffer.Create(sizeof(Vec3), static_cast<int>(FrameAllVertices.size()),
	                    SB_TYPE::SRV_ONLY, false, FrameAllVertices.data());
	AllIdxBuffer.Create(sizeof(UINT), static_cast<int>(FrameAllIndices.size()),
	                    SB_TYPE::SRV_ONLY, false, FrameAllIndices.data());
	TasksBuffer.Create(sizeof(tRaycastTask), static_cast<int>(RaycastTasks.size()),
	                   SB_TYPE::SRV_ONLY, false, RaycastTasks.data());
	ResultsBuffer.Create(sizeof(RaycastResult), static_cast<int>(RaycastTasks.size()),
	                     SB_TYPE::SRV_UAV, true);

	// 2. 컴퓨트 셰이더 설정 및 실행
	RaycastCS.SetVertexAndIndexBuffers(&AllVtxBuffer, &AllIdxBuffer);
	RaycastCS.SetTaskBuffer(&TasksBuffer);
	RaycastCS.SetResultBuffer(&ResultsBuffer);
	RaycastCS.SetTaskCount(static_cast<int>(RaycastTasks.size()));
	RaycastCS.Execute();

	// 3. GPU로부터 결과 데이터 가져오기
	vector<RaycastResult> Result(RaycastTasks.size());
	ResultsBuffer.GetData(Result.data());

	// 4. 결과 처리
	for (size_t i = 0; i < Result.size(); ++i)
	{
		if (Result[i].IsHit)
		{
			const auto& Colliders = RaycastTaskColliders[i];

			FColliderRay* Ray = Colliders.first;
			FMeshCollider* Mesh = Colliders.second;

			// 충돌 정보를 레이 콜라이더에 설정
			// Ray->SetHitDistance(results[i].Distance);
			// Ray->SetHitNormal(results[i].HitNormal);

			// 최종 충돌 처리 로직 호출
			AddFrameCollision(Ray, Mesh);
		}
	}
}
