#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider2D.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/Runtime/Public/Component/Physics/CMeshCollider.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

/*************************/
/** Constant Structures **/
/*************************/

constexpr UINT Indices[36] = {
	0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6,
	0, 4, 5, 0, 5, 1, 1, 5, 6, 1, 6, 2,
	2, 6, 7, 2, 7, 3, 3, 7, 4, 3, 4, 0
};

constexpr Vec3 UnitCube[8] = {
	{-0.5f, 0.5f, -0.5f},
	{0.5f, 0.5f, -0.5f},
	{0.5f, -0.5f, -0.5f},
	{-0.5f, -0.5f, -0.5f},
	{-0.5f, 0.5f, 0.5f},
	{0.5f, 0.5f, 0.5f},
	{0.5f, -0.5f, 0.5f},
	{-0.5f, -0.5f, 0.5f},
};

/**
 * @brief 충돌 가능성이 있는 오브젝트 쌍에 대해 충돌 판정을 처리하는 함수
 */
void FCollisionManager::NarrowPhase()
{
	// Reset Task Data
	MTasks.clear();
	MTaskColliders.clear();
	MFrameAllVertices.clear();
	MFrameAllIndices.clear();
	MDataCache.clear();

	// CPU와 GPU 처리 대상을 분리
	for (auto& Pair : MCandidatePairVector)
	{
		// CS 처리가 필요한 경우 CS Task에만 달고 나감
		if (NeedComputeShader(Pair.first, Pair.second))
		{
			AddShaderTask(Pair.first, Pair.second);
		}
		// CS 처리 대상이 아닐 경우, 여기서 충돌 처리 완료
		else
		{
			CheckPair(Pair.first, Pair.second);
		}
	}

	// CS Task Batch Process
	if (!MTasks.empty())
	{
		// LOG_INFO_F("[Collision][ComputeShader] True Collision After CS Check: {}", MTasks.size());
		ExecuteAndProcessCS();
	}
}

/**
 * @brief 세부 충돌 검사에 Compute Shader 처리가 필요한지 판별하는 함수
 * @return CS 처리가 필요하다면 True
 */
bool FCollisionManager::NeedComputeShader(const CGameObject* ObjectA, const CGameObject* ObjectB)
{
	const bool IsMeshA = (ObjectA->MeshCollider() != nullptr);
	const bool IsMeshB = (ObjectB->MeshCollider() != nullptr);
	const bool Is3DColliderA = (ObjectA->Collider3D() != nullptr);
	const bool Is3DColliderB = (ObjectB->Collider3D() != nullptr);

	if ((IsMeshA && IsMeshB) || (IsMeshA && Is3DColliderB) || (Is3DColliderA && IsMeshB))
	{
		return true;
	}

	return false;
}

/**
 * @brief MeshCollider 정보를 Buffer에 추가하는 함수
 */
FCollisionManager::MeshBatchData FCollisionManager::GetOrAddBatchData(const CMeshCollider* PCollider)
{
	// 중복 생성 방지
	const void* ColliderKey = PCollider;
	if (MDataCache.contains(ColliderKey))
	{
		return MDataCache.at(ColliderKey);
	}

	MeshBatchData NewData = {};
	NewData.VertexOffset = static_cast<UINT>(MFrameAllVertices.size());
	NewData.IndexOffset = static_cast<UINT>(MFrameAllIndices.size());

	Ptr<CMesh> Mesh = PCollider->GetMesh();
	if (Mesh.Get())
	{
		Matrix WorldMatrix = PCollider->GetOwner()->Transform()->GetWorldMat();
		Vtx* Vertices = static_cast<Vtx*>(Mesh->GetVtxSysMem());
		UINT VertexCount = Mesh->GetVertexCount();

		for (UINT i = 0; i < VertexCount; ++i)
		{
			MFrameAllVertices.push_back(XMVector3TransformCoord(Vertices[i].vPos, WorldMatrix));
		}

		UINT TotalTriangleCount = 0;
		for (UINT i = 0; i < Mesh->GetSubsetCount(); ++i)
		{
			const auto& Subset = Mesh->GetIndexInfo()[i];
			UINT* Indices = static_cast<UINT*>(Subset.IdxSysMem);

			for (UINT j = 0; j < Subset.IdxCount; ++j)
			{
				MFrameAllIndices.push_back(Indices[j]);
			}

			TotalTriangleCount += Subset.IdxCount / 3;
		}

		NewData.TriCount = TotalTriangleCount;
	}

	// Cache에 등록
	MDataCache[ColliderKey] = NewData;

	return NewData;
}

/**
 * @brief Collider3D 정보를 Buffer에 추가하는 함수
 */
FCollisionManager::MeshBatchData FCollisionManager::GetOrAddBatchData(const CCollider3D* PCollider)
{
	// 중복 생성 방지
	const void* ColliderKey = PCollider;
	if (MDataCache.contains(ColliderKey))
	{
		return MDataCache.at(ColliderKey);
	}

	MeshBatchData NewData;
	NewData.VertexOffset = static_cast<UINT>(MFrameAllVertices.size());
	NewData.IndexOffset = static_cast<UINT>(MFrameAllIndices.size());

	Matrix WorldMatrix = PCollider->GetColliderWorldMat();
	Vec3 Vertices[8];
	for (int i = 0; i < 8; ++i)
	{
		Vertices[i] = XMVector3Transform(UnitCube[i], WorldMatrix);
	}

	// Add Data
	for (int i = 0; i < 8; ++i)
	{
		MFrameAllVertices.push_back(Vertices[i]);
	}
	for (int i = 0; i < 36; ++i)
	{
		MFrameAllIndices.push_back(Indices[i]);
	}
	NewData.TriCount = 12;

	// Cache에 등록
	MDataCache[ColliderKey] = NewData;

	return NewData;
}

/**
 * @brief 배치 처리를 실행하고 결과를 처리하는 함수
 */
void FCollisionManager::ExecuteAndProcessCS()
{
	// Create Structured Buffer
	CStructuredBuffer AllVtxBuffer, AllIdxBuffer, TasksBuffer, ResultsBuffer;
	AllVtxBuffer.Create(sizeof(Vec3), static_cast<UINT>(MFrameAllVertices.size()),
	                    SB_TYPE::SRV_ONLY, false, MFrameAllVertices.data());
	AllIdxBuffer.Create(sizeof(UINT), static_cast<UINT>(MFrameAllIndices.size()),
	                    SB_TYPE::SRV_ONLY, false, MFrameAllIndices.data());
	TasksBuffer.Create(sizeof(tCollisionTask), static_cast<UINT>(MTasks.size()),
	                   SB_TYPE::SRV_ONLY, false, MTasks.data());
	ResultsBuffer.Create(sizeof(CollisionResult), static_cast<UINT>(MTasks.size()),
	                     SB_TYPE::SRV_UAV, true);

	// Setting CS & Execution (Clear Included)
	MMeshCollisionCS.SetVertexAndIndexBuffers(&AllVtxBuffer, &AllIdxBuffer);
	MMeshCollisionCS.SetTaskBuffer(&TasksBuffer);
	MMeshCollisionCS.SetResultBuffer(&ResultsBuffer);
	MMeshCollisionCS.SetTaskCount(static_cast<int>(MTasks.size()));
	MMeshCollisionCS.Execute();

	// Get Results
	vector<CollisionResult> Results(MTasks.size());
	ResultsBuffer.GetData(Results.data());

	// Process If Collided
	for (size_t i = 0; i < Results.size(); ++i)
	{
		if (Results[i].Collided)
		{
			visit([&](auto Pair)
			{
				Pair.first->SetCollisionNormal(Results[i].LeftNormal);
				Pair.first->SetPenetrationDepth(Results[i].PenetrationDepth);
				Pair.second->SetCollisionNormal(Results[i].RightNormal);
				Pair.second->SetPenetrationDepth(Results[i].PenetrationDepth);

				AddFrameCollision(Pair.first, Pair.second);
			}, MTaskColliders[i]);
		}
	}
}

/**
 * @brief Task 추가 진행하면서 Object의 충돌체 관련 정보도 동시에 저장하는 함수
 * @param PLeftObject Collision Object 1
 * @param PRightObject Collision Object 2
 */
void FCollisionManager::AddShaderTask(const CGameObject* PLeftObject, const CGameObject* PRightObject)
{
	CMeshCollider* LeftMesh = PLeftObject->MeshCollider();
	CMeshCollider* RightMesh = PRightObject->MeshCollider();
	CCollider3D* Left3D = PLeftObject->Collider3D();
	CCollider3D* Right3D = PRightObject->Collider3D();

	MeshBatchData LeftData, RightData;

	if (LeftMesh && RightMesh)
	{
		LeftData = GetOrAddBatchData(LeftMesh);
		RightData = GetOrAddBatchData(RightMesh);
		MTaskColliders.emplace_back(make_pair(LeftMesh, RightMesh));
	}
	else if (LeftMesh && Right3D)
	{
		LeftData = GetOrAddBatchData(LeftMesh);
		RightData = GetOrAddBatchData(Right3D);
		MTaskColliders.emplace_back(make_pair(LeftMesh, Right3D));
	}
	else if (Left3D && RightMesh)
	{
		LeftData = GetOrAddBatchData(Left3D);
		RightData = GetOrAddBatchData(RightMesh);
		MTaskColliders.emplace_back(make_pair(Left3D, RightMesh));
	}
	else
	{
		return;
	}

	// 2. CS 작업 생성 및 추가
	tCollisionTask Task = {};
	Task.LeftVertexOffset = LeftData.VertexOffset;
	Task.LeftIndexOffset = LeftData.IndexOffset;
	Task.LeftTriCount = LeftData.TriCount;
	Task.RightVertexOffset = RightData.VertexOffset;
	Task.RightIndexOffset = RightData.IndexOffset;
	Task.RightTriCount = RightData.TriCount;

	MTasks.push_back(Task);
}

/**
 * @brief Narrow 충돌 판정 중 CPU 기반의 판정을 진행하고, 충돌했다면 적절한 처리 로직을 호출하는 함수
 *
 * @param PLeftObject Object 1
 * @param PRightObject Object 2
 */
void FCollisionManager::CheckPair(const CGameObject* PRightObject, const CGameObject* PLeftObject)
{
	// 2D 충돌체 검사
	if (PLeftObject->Collider2D())
	{
		if (PRightObject->Collider2D())
		{
			if (IsCollision(PLeftObject->Collider2D(), PRightObject->Collider2D()))
			{
				AddFrameCollision(PLeftObject->Collider2D(), PRightObject->Collider2D());
			}
		}
	}

	// 3D 충돌체 검사
	if (PLeftObject->Collider3D())
	{
		if (PRightObject->Collider3D())
		{
			if (IsCollision(PLeftObject->Collider3D(), PRightObject->Collider3D()))
			{
				AddFrameCollision(PLeftObject->Collider3D(), PRightObject->Collider3D());
			}
		}

		if (PRightObject->LandScape())
		{
			if (IsCollision(PLeftObject->Collider3D(), PRightObject->LandScape()))
			{
				AddFrameCollision(PLeftObject->Collider3D(), PRightObject->LandScape());
			}
		}
	}

	// LandScape 검사
	if (PLeftObject->LandScape())
	{
		if (PRightObject->Collider3D())
		{
			if (IsCollision(PRightObject->Collider3D(), PLeftObject->LandScape()))
			{
				AddFrameCollision(PLeftObject->LandScape(), PRightObject->Collider3D());
			}
		}
	}

	// LOG_INFO_F("[Collision][Narrow] Object {} & {}, Failed To Match Type Pairing",
	//            WStringToString(PLeftObject->GetName()), WStringToString(PRightObject->GetName()));
}

void FCollisionManager::AddFrameCollision(ColliderVariant PLeftCollider, ColliderVariant PRightCollider)
{
	visit([&](auto* LeftCollider, auto* RightCollider)
	{
		MFrameCollisionSet.insert(COLLISION_ID(LeftCollider->GetID(), RightCollider->GetID()).ID);
	}, PLeftCollider, PRightCollider);
}

/**********************************/
/** Narrow Collision Check Logic **/
/**********************************/

// TODO(KHJ): 이하의 코드 재검토 필요

bool FCollisionManager::IsCollision(const CCollider2D* PLeftCollider, const CCollider2D* PRightCollider)
{
	// SAT(Separating Axis Theorem) 기반 2D 충돌 판정
	static Vec3 arrRect[4] = {
		Vec3(-0.5f, 0.5f, 0.f), Vec3(0.5f, 0.5f, 0.f), Vec3(0.5f, -0.5f, 0.f), Vec3(-0.5f, -0.5f, 0.f)
	};
	Matrix matColLeft = PLeftCollider->GetColliderWorldMatrix();
	Matrix matColRight = PRightCollider->GetColliderWorldMatrix();
	Vec3 arrProj[4] = {};
	arrProj[0] = XMVector3TransformCoord(arrRect[1], matColLeft) - XMVector3TransformCoord(arrRect[0], matColLeft);
	arrProj[1] = XMVector3TransformCoord(arrRect[3], matColLeft) - XMVector3TransformCoord(arrRect[0], matColLeft);
	arrProj[2] = XMVector3TransformCoord(arrRect[1], matColRight) - XMVector3TransformCoord(arrRect[0], matColRight);
	arrProj[3] = XMVector3TransformCoord(arrRect[3], matColRight) - XMVector3TransformCoord(arrRect[0], matColRight);
	Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColLeft) - XMVector3TransformCoord(
		Vec3(0.f, 0.f, 0.f), matColRight);
	for (int i = 0; i < 4; ++i)
	{
		Vec3 vProj = arrProj[i];
		vProj.Normalize();
		float fCenter = fabs(vCenter.Dot(vProj));
		float fDist = 0.f;
		for (int j = 0; j < 4; ++j)
			fDist += fabs(vProj.Dot(arrProj[j]));
		fDist /= 2.f;
		if (fDist < fCenter)
			return false;
	}
	return true;
}

bool FCollisionManager::IsCollision(const CCollider3D* PLeftCollider, const CCollider3D* PRightCollider)
{
	constexpr float EPSILON = 0.0001f;
	static Vec3 arrCube[8] = {
		Vec3(-0.5f, 0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f), Vec3(0.5f, -0.5f, 0.5f), Vec3(-0.5f, -0.5f, 0.5f),
		Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f), Vec3(0.5f, -0.5f, -0.5f), Vec3(-0.5f, -0.5f, -0.5f)
	};
	Matrix matColLeft = PLeftCollider->GetColliderWorldMat();
	Matrix matColRight = PRightCollider->GetColliderWorldMat();
	Vec3 leftCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColLeft);
	Vec3 rightCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColRight);
	Vec3 leftVertices[8], rightVertices[8];
	for (int i = 0; i < 8; i++)
	{
		leftVertices[i] = XMVector3TransformCoord(arrCube[i], matColLeft);
		rightVertices[i] = XMVector3TransformCoord(arrCube[i], matColRight);
	}
	Vec3 axes[15];
	int axisCount = 0;
	Vec3 leftAxis[3] = {
		XMVector3TransformNormal(Vec3(1.0f, 0.0f, 0.0f), matColLeft),
		XMVector3TransformNormal(Vec3(0.0f, 1.0f, 0.0f), matColLeft),
		XMVector3TransformNormal(Vec3(0.0f, 0.0f, 1.0f), matColLeft)
	};
	Vec3 rightAxis[3] = {
		XMVector3TransformNormal(Vec3(1.0f, 0.0f, 0.0f), matColRight),
		XMVector3TransformNormal(Vec3(0.0f, 1.0f, 0.0f), matColRight),
		XMVector3TransformNormal(Vec3(0.0f, 0.0f, 1.0f), matColRight)
	};
	for (int i = 0; i < 3; i++)
	{
		leftAxis[i].Normalize();
		rightAxis[i].Normalize();
		axes[axisCount++] = leftAxis[i];
		axes[axisCount++] = rightAxis[i];
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Vec3 crossAxis = leftAxis[i].Cross(rightAxis[j]);
			if (crossAxis.Length() > EPSILON)
			{
				crossAxis.Normalize();
				axes[axisCount++] = crossAxis;
			}
		}
	}
	for (int a = 0; a < axisCount; ++a)
	{
		float leftMin = FLT_MAX, leftMax = -FLT_MAX, rightMin = FLT_MAX, rightMax = -FLT_MAX;
		for (int i = 0; i < 8; i++)
		{
			float dot = leftVertices[i].Dot(axes[a]);
			leftMin = min(leftMin, dot);
			leftMax = max(leftMax, dot);
		}
		for (int i = 0; i < 8; i++)
		{
			float dot = rightVertices[i].Dot(axes[a]);
			rightMin = min(rightMin, dot);
			rightMax = max(rightMax, dot);
		}
		if (leftMin > rightMax + EPSILON || rightMin > leftMax + EPSILON)
			return false;
	}
	return true;
}

bool FCollisionManager::IsCollision(const CCollider3D* PLeftCollider, const CLandScape* PRightCollider)
{
	Vec3 ObjectPos = PLeftCollider->Transform()->GetWorldPos();
	Vec3 LandScapePos = PRightCollider->GetWorldPosByLandScape(ObjectPos);
	if (LandScapePos == Vec3(-10000.f, -10000.f, -10000.f) || ObjectPos.y > LandScapePos.y)
		return false;
	return true;
}
