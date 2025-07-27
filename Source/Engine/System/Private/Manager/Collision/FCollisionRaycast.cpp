#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "Engine/Runtime/Public/Component/Physics/Collider3D.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Physics/CMeshCollider.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

/*******************/
/** Raycast Check **/
/*******************/

/**
 * @brief 레벨 내 모든 Ray Collider와 충돌 가능한 오브젝트에 대해 Raycast를 처리하는 함수
 */
void CollisionManager::RaycastProcess()
{
	// 모든 Ray Collider 수집
	vector<CColliderRay*> RayColliders;
	for (const auto* Object : LevelActiveObjects)
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
		QueryBVH(BVHRootNode, Ray, Candidates);

		for (const RayColliderInfo& Info : Candidates)
		{
			auto* Object = Info.HitObject;

			// AABB가 충돌 기준이므로 바로 Process
			if (Object->Collider3D())
			{
				AddFrameCollision(Ray, Object->Collider3D());
				continue;
			}
			if (Object->LandScape())
			{
				AddFrameCollision(Ray, Object->LandScape());
				continue;
			}

			// RaycastCS 처리를 위한 Add Task
			if (Object->MeshCollider())
			{
				AddRayShaderTask(Ray, Object);
			}
		}
	}
}

/**
 * @brief CS 처리를 위해 MeshCollider의 지오메트리 데이터를 전역 버퍼에 추가하고 정보를 반환하는 함수
 */
CollisionManager::MeshBatchData CollisionManager::GetOrAddRaycastBatchData(const CMeshCollider* InCollider)
{
	// 중복 방지
	if (RaycastDataCache.contains(InCollider))
	{
		return RaycastDataCache.at(InCollider);
	}

	// 캐시에 없다면 새로 데이터를 추가
	MeshBatchData NewData = {};
	NewData.VertexOffset = static_cast<UINT>(FrameAllVertices.size());
	NewData.IndexOffset = static_cast<UINT>(FrameAllIndices.size());

	Ptr<CMesh> MeshPtr = InCollider->GetMesh();
	if (MeshPtr.Get())
	{
		Matrix WorldMatrix = InCollider->GetOwner()->Transform()->GetWorldMat();
		Vtx* Vertices = static_cast<Vtx*>(MeshPtr->GetVtxSysMem());
		UINT VertexCount = MeshPtr->GetVertexCount();

		for (UINT i = 0; i < VertexCount; ++i)
		{
			FrameAllVertices.push_back(XMVector3TransformCoord(Vertices[i].vPos, WorldMatrix));
		}

		UINT TotalTriCount = 0;
		for (UINT i = 0; i < MeshPtr->GetSubsetCount(); ++i)
		{
			const auto& Subset = MeshPtr->GetIndexInfo()[i];
			UINT* Indices = static_cast<UINT*>(Subset.IdxSysMem);
			for (UINT j = 0; j < Subset.IdxCount; ++j)
			{
				FrameAllIndices.push_back(Indices[j]);
			}
			TotalTriCount += Subset.IdxCount / 3;
		}
		NewData.TriCount = TotalTriCount;
	}

	// 데이터 저장 후 반환
	RaycastDataCache[InCollider] = NewData;
	return NewData;
}

void CollisionManager::AddRayShaderTask(CColliderRay* InRay, const CGameObject* InObject)
{
	MeshBatchData MeshData = GetOrAddRaycastBatchData(InObject->MeshCollider());
	RaycastColliders.push_back(make_pair(InRay, InObject->MeshCollider()));

	tRaycastTask Task = {};
	Task.RayOrigin = InRay->GetRayFinalPos();
	Task.RayDirection = InRay->GetRayFinalDir();
	Task.VertexOffset = MeshData.VertexOffset;
	Task.IndexOffset = MeshData.IndexOffset;
	Task.TriCount = MeshData.TriCount;

	RaycastTasks.push_back(Task);
}

/**
 * @brief 레이캐스트 배치 처리를 실행하고 결과를 처리하는 함수
 */
void CollisionManager::ExecuteAndProcessRaycastCS()
{
	if (RaycastTasks.empty()) return;

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
	std::vector<RaycastResult> results(RaycastTasks.size());
	ResultsBuffer.GetData(results.data());

	// 4. 결과 처리
	for (size_t i = 0; i < results.size(); ++i)
	{
		if (results[i].IsHit)
		{
			auto& Colliders = RaycastColliders[i];
			CColliderRay* Ray = Colliders.first;
			CMeshCollider* Mesh = Colliders.second;

			// 충돌 정보를 레이 콜라이더에 설정
			// Ray->SetHitDistance(results[i].Distance);
			// Ray->SetHitNormal(results[i].HitNormal);

			// 최종 충돌 처리 로직 호출
			AddFrameCollision(Ray, Mesh);
		}
	}
}
