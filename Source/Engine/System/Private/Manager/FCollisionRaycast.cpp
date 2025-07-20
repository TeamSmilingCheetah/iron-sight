#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Physics/CMeshCollider.h"
#include "Runtime/Public/Component/Transform/CTransform.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

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

			// AABB가 충돌 기준이므로 바로 Process
			if (Object->Collider3D())
			{
				ProcessCollision(Ray, Object->Collider3D());
				continue;
			}
			if (Object->LandScape())
			{
				ProcessCollision(Ray, Object->LandScape());
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
 * @brief CS 처리를 위해 MeshCollider의 지오메트리 데이터를 전역 버퍼에 추가하고 정보를 반환합니다. (레이캐스트용)
 */
FCollisionManager::MeshBatchData FCollisionManager::GetOrAddRaycastBatchData(CMeshCollider* pCollider)
{
	// 중복 방지
	if (MRaycastDataCache.contains(pCollider))
	{
		return MRaycastDataCache.at(pCollider);
	}

	// 2. 캐시에 없다면 새로 데이터를 추가
	MeshBatchData newData = {};
	newData.VertexOffset = static_cast<UINT>(MFrameAllVertices.size());
	newData.IndexOffset = static_cast<UINT>(MFrameAllIndices.size());

	Ptr<CMesh> pMesh = pCollider->GetMesh();
	if (pMesh.Get())
	{
		Matrix worldMat = pCollider->GetOwner()->Transform()->GetWorldMat();
		Vtx* pVertices = static_cast<Vtx*>(pMesh->GetVtxSysMem());
		UINT vertexCount = pMesh->GetVertexCount();

		for (UINT i = 0; i < vertexCount; ++i)
		{
			MFrameAllVertices.push_back(XMVector3TransformCoord(pVertices[i].vPos, worldMat));
		}

		UINT totalTriCount = 0;
		for (UINT i = 0; i < pMesh->GetSubsetCount(); ++i)
		{
			const auto& subset = pMesh->GetIndexInfo()[i];
			UINT* pIndices = static_cast<UINT*>(subset.IdxSysMem);
			for (UINT j = 0; j < subset.IdxCount; ++j)
			{
				MFrameAllIndices.push_back(pIndices[j]);
			}
			totalTriCount += subset.IdxCount / 3;
		}
		newData.TriCount = totalTriCount;
	}

	// 3. 캐시에 데이터 저장 후 반환
	MRaycastDataCache[pCollider] = newData;
	return newData;
}

void FCollisionManager::AddRayShaderTask(CColliderRay* PRay, const CGameObject* PObject)
{
	MeshBatchData MeshData = GetOrAddRaycastBatchData(PObject->MeshCollider());
	MRaycastColliders.push_back(make_pair(PRay, PObject->MeshCollider()));

	tRaycastTask Task = {};
	Task.RayOrigin = PRay->GetWorldPosition();
	Task.RayDirection = PRay->GetWorldDirection();
	Task.VertexOffset = MeshData.VertexOffset;
	Task.IndexOffset = MeshData.IndexOffset;
	Task.TriCount = MeshData.TriCount;

	MRaycastTasks.push_back(Task);
}

/**
 * @brief 레이캐스트 배치 처리를 실행하고 결과를 처리하는 함수
 */
void FCollisionManager::ExecuteAndProcessRaycastCS()
{
	if (MRaycastTasks.empty()) return;

	// 1. GPU에 보낼 구조화된 버퍼 생성
	CStructuredBuffer AllVtxBuffer, AllIdxBuffer, TasksBuffer, ResultsBuffer;
	AllVtxBuffer.Create(sizeof(Vec3), static_cast<int>(MFrameAllVertices.size()),
	                    SB_TYPE::SRV_ONLY, false, MFrameAllVertices.data());
	AllIdxBuffer.Create(sizeof(UINT), static_cast<int>(MFrameAllIndices.size()),
	                    SB_TYPE::SRV_ONLY, false, MFrameAllIndices.data());
	TasksBuffer.Create(sizeof(tRaycastTask), static_cast<int>(MRaycastTasks.size()),
	                   SB_TYPE::SRV_ONLY, false, MRaycastTasks.data());
	ResultsBuffer.Create(sizeof(RaycastResult), static_cast<int>(MRaycastTasks.size()),
	                     SB_TYPE::SRV_UAV, true);

	// 2. 컴퓨트 셰이더 설정 및 실행
	MRaycastCS.SetVertexAndIndexBuffers(&AllVtxBuffer, &AllIdxBuffer);
	MRaycastCS.SetTaskBuffer(&TasksBuffer);
	MRaycastCS.SetResultBuffer(&ResultsBuffer);
	MRaycastCS.SetTaskCount(static_cast<int>(MRaycastTasks.size()));
	MRaycastCS.Execute();

	// 3. GPU로부터 결과 데이터 가져오기
	std::vector<RaycastResult> results(MRaycastTasks.size());
	ResultsBuffer.GetData(results.data());

	// 4. 결과 처리
	for (size_t i = 0; i < results.size(); ++i)
	{
		if (results[i].IsHit)
		{
			auto& Colliders = MRaycastColliders[i];
			CColliderRay* Ray = Colliders.first;
			CMeshCollider* Mesh = Colliders.second;

			// 충돌 정보를 레이 콜라이더에 설정
			Ray->SetHitDistance(results[i].Distance);
			Ray->SetHitNormal(results[i].HitNormal);

			// 최종 충돌 처리 로직 호출
			ProcessCollision(Ray, Mesh);
		}
	}
}
