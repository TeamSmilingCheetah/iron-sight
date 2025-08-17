#include "pch.h"
#include "Engine/System/Public/Manager/CollisionManager.h"

#include "Engine/System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"
#include "Engine/Runtime/Public/Component/Physics/RayCollider.h"
#include "Engine/Runtime/Public/Component/Physics/MeshCollider.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

using std::ranges::sort;

/*******************/
/** Raycast Check **/
/*******************/

/**
 * @brief BVH를 순회하며 교차하는 AABB를 후보군에 등록하는 함수
 */
void FCollisionManager::RaycastBroad()
{
	// Check All Collision
	for (FRayCollider* Ray : RayColliders)
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
	map<FRayCollider*, vector<pair<IColliderBase*, float>>> RayHitMap;

	for (const FRayCollisionInfo& Info : RayCandidates)
	{
		FRayCollider* Ray = Info.RayObject;
		IColliderBase* Collider = Info.HitCollider;

		unordered_set<FCollisionID> CheckSet;
		if (IsInCondition(CheckSet, Ray, Collider))
		{
			// AABB가 충돌 기준이므로 바로 Process
			if (Collider->GetColliderType() == EColliderType::BoxCollider)
			{
				// 교차점 거리 계산
				AABB BoxAABB = Collider->GetAABB();
				float HitDistance = 0.0f;

				// Ray와 AABB의 교차 여부 및 거리 계산
				if (BoxAABB.Intersects(Ray->GetFinalPosition(), Ray->GetFinalDirection(), &HitDistance))
				{
					// 음수 거리는 Ray 뒤쪽 교차점이므로 제외해야 함
					if (HitDistance >= 0.0f && HitDistance <= Ray->GetLength())
					{
						RayHitMap[Ray].push_back(make_pair(Collider, HitDistance));
					}
				}

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

	for (auto& [Ray, Hits] : RayHitMap)
	{
		// 거리 순으로 정렬
		sort(Hits, [](const pair<IColliderBase*, float>& ColliderA, const pair<IColliderBase*, float>& ColliderB)
		{
			return ColliderA.second < ColliderB.second;
		});

		// TargetAll 옵션이 켜져 있는 경우, 관통한 모든 Hit을 처리해야 함
		// 그렇지 않다면 처음 충돌한 Hit만 처리
		for (size_t i = 0; i < Hits.size(); ++i)
		{
			if (Hits[i].second <= Ray->GetLength())
			{
				Ray->SetHitDistance(Hits[i].second);
				// TODO(KHJ): Box Collider에 대해서 Normal 필요할진 모르겠지만 필요하면 반환할 수 있도록 처리해야 함
				Ray->SetHitNormal(Vec3(0, 1, 0));
				AddFrameCollision(Ray, static_cast<FBoxCollider*>(Hits[i].first));
			}

			if (!Ray->IsTargetAllMode())
			{
				break;
			}
		}
	}
}

/**
 * @brief 충돌체와 실제로 충돌하는지 확인하기 위한 CS Task에 추가하는 함수
 *
 * @param InRay Ray Collider
 * @param InCollider Other Collider (Mesh)
 */
void FCollisionManager::AddRayShaderTask(FRayCollider* InRay, const FMeshCollider* InCollider)
{
	FMeshBatchData MeshData = GetOrAddBatchData(InCollider);
	RaycastTaskColliders.push_back(make_pair(InRay, InCollider->MeshCollider()));

	tRaycastTask Task = {};
	Task.RayOrigin = InRay->GetFinalPosition();
	Task.RayDirection = InRay->GetFinalDirection();
	Task.VertexOffset = MeshData.VertexOffset;
	Task.IndexOffset = MeshData.IndexOffset;
	Task.TriCount = MeshData.TriangleCount;

	RaycastTasks.push_back(Task);
}

/**
 * @brief Raycast Batch Process를 실행하고 결과를 처리하는 함수
 * // TODO(KHJ): TargetAll 미구현, 해당 부분을 고려한 다중 처리를 할 수 있어야 할 듯
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

			FRayCollider* Ray = Colliders.first;
			FMeshCollider* Mesh = Colliders.second;

			if (Result[i].Distance <= Ray->GetLength())
			{
				// 충돌 정보를 레이 콜라이더에 설정
				Ray->SetHitDistance(Result[i].Distance);
				Ray->SetHitNormal(Result[i].HitNormal);

				// 최종 충돌 처리 로직 호출
				AddFrameCollision(Ray, Mesh);
			}
		}
	}
}
