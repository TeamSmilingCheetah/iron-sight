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
void CollisionManager::NarrowPhase()
{
	// Reset Task Data
	Tasks.clear();
	TaskColliders.clear();
	FrameAllVertices.clear();
	FrameAllIndices.clear();
	DataCache.clear();

	// CPU와 GPU 처리 대상을 분리
	for (auto& Pair : CandidatePairVector)
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
	if (!Tasks.empty())
	{
		// LOG_INFO_F("[Collision][ComputeShader] True Collision After CS Check: {}", MTasks.size());
		ExecuteAndProcessCS();
	}
}

/**
 * @brief 세부 충돌 검사에 Compute Shader 처리가 필요한지 판별하는 함수
 * @return CS 처리가 필요하다면 True
 */
bool CollisionManager::NeedComputeShader(const CGameObject* ObjectA, const CGameObject* ObjectB)
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
CollisionManager::MeshBatchData CollisionManager::GetOrAddBatchData(const CMeshCollider* InCollider)
{
	// 중복 생성 방지
	const void* ColliderKey = InCollider;
	if (DataCache.contains(ColliderKey))
	{
		return DataCache.at(ColliderKey);
	}

	MeshBatchData NewData = {};
	NewData.VertexOffset = static_cast<UINT>(FrameAllVertices.size());
	NewData.IndexOffset = static_cast<UINT>(FrameAllIndices.size());

	Ptr<CMesh> Mesh = InCollider->GetMesh();
	if (Mesh.Get())
	{
		Matrix WorldMatrix = InCollider->GetOwner()->Transform()->GetWorldMat();
		Vtx* Vertices = static_cast<Vtx*>(Mesh->GetVtxSysMem());
		UINT VertexCount = Mesh->GetVertexCount();

		for (UINT i = 0; i < VertexCount; ++i)
		{
			FrameAllVertices.push_back(XMVector3TransformCoord(Vertices[i].vPos, WorldMatrix));
		}

		UINT TotalTriangleCount = 0;
		for (UINT i = 0; i < Mesh->GetSubsetCount(); ++i)
		{
			const auto& Subset = Mesh->GetIndexInfo()[i];
			UINT* Indices = static_cast<UINT*>(Subset.IdxSysMem);

			for (UINT j = 0; j < Subset.IdxCount; ++j)
			{
				FrameAllIndices.push_back(Indices[j]);
			}

			TotalTriangleCount += Subset.IdxCount / 3;
		}

		NewData.TriCount = TotalTriangleCount;
	}

	// Cache에 등록
	DataCache[ColliderKey] = NewData;

	return NewData;
}

/**
 * @brief Collider3D 정보를 Buffer에 추가하는 함수
 */
CollisionManager::MeshBatchData CollisionManager::GetOrAddBatchData(const CCollider3D* InCollider)
{
	// 중복 생성 방지
	const void* ColliderKey = InCollider;
	if (DataCache.contains(ColliderKey))
	{
		return DataCache.at(ColliderKey);
	}

	MeshBatchData NewData;
	NewData.VertexOffset = static_cast<UINT>(FrameAllVertices.size());
	NewData.IndexOffset = static_cast<UINT>(FrameAllIndices.size());

	Matrix WorldMatrix = InCollider->GetColliderWorldMat();
	Vec3 Vertices[8];
	for (int i = 0; i < 8; ++i)
	{
		Vertices[i] = XMVector3Transform(UnitCube[i], WorldMatrix);
	}

	// Add Data
	for (int i = 0; i < 8; ++i)
	{
		FrameAllVertices.push_back(Vertices[i]);
	}
	for (int i = 0; i < 36; ++i)
	{
		FrameAllIndices.push_back(Indices[i]);
	}
	NewData.TriCount = 12;

	// Cache에 등록
	DataCache[ColliderKey] = NewData;

	return NewData;
}

/**
 * @brief 배치 처리를 실행하고 결과를 처리하는 함수
 */
void CollisionManager::ExecuteAndProcessCS()
{
	// Create Structured Buffer
	CStructuredBuffer AllVtxBuffer, AllIdxBuffer, TasksBuffer, ResultsBuffer;
	AllVtxBuffer.Create(sizeof(Vec3), static_cast<UINT>(FrameAllVertices.size()),
	                    SB_TYPE::SRV_ONLY, false, FrameAllVertices.data());
	AllIdxBuffer.Create(sizeof(UINT), static_cast<UINT>(FrameAllIndices.size()),
	                    SB_TYPE::SRV_ONLY, false, FrameAllIndices.data());
	TasksBuffer.Create(sizeof(tCollisionTask), static_cast<UINT>(Tasks.size()),
	                   SB_TYPE::SRV_ONLY, false, Tasks.data());
	ResultsBuffer.Create(sizeof(CollisionResult), static_cast<UINT>(Tasks.size()),
	                     SB_TYPE::SRV_UAV, true);

	// Setting CS & Execution (Clear Included)
	MeshCollisionCS.SetVertexAndIndexBuffers(&AllVtxBuffer, &AllIdxBuffer);
	MeshCollisionCS.SetTaskBuffer(&TasksBuffer);
	MeshCollisionCS.SetResultBuffer(&ResultsBuffer);
	MeshCollisionCS.SetTaskCount(static_cast<int>(Tasks.size()));
	MeshCollisionCS.Execute();

	// Get Results
	vector<CollisionResult> Results(Tasks.size());
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
			}, TaskColliders[i]);
		}
	}
}

/**
 * @brief Task 추가 진행하면서 Object의 충돌체 관련 정보도 동시에 저장하는 함수
 * @param InLeftObject Collision Object 1
 * @param InRightObject Collision Object 2
 */
void CollisionManager::AddShaderTask(const CGameObject* InLeftObject, const CGameObject* InRightObject)
{
	CMeshCollider* LeftMesh = InLeftObject->MeshCollider();
	CMeshCollider* RightMesh = InRightObject->MeshCollider();
	CCollider3D* Left3D = InLeftObject->Collider3D();
	CCollider3D* Right3D = InRightObject->Collider3D();

	MeshBatchData LeftData, RightData;

	if (LeftMesh && RightMesh)
	{
		LeftData = GetOrAddBatchData(LeftMesh);
		RightData = GetOrAddBatchData(RightMesh);
		TaskColliders.emplace_back(make_pair(LeftMesh, RightMesh));
	}
	else if (LeftMesh && Right3D)
	{
		LeftData = GetOrAddBatchData(LeftMesh);
		RightData = GetOrAddBatchData(Right3D);
		TaskColliders.emplace_back(make_pair(LeftMesh, Right3D));
	}
	else if (Left3D && RightMesh)
	{
		LeftData = GetOrAddBatchData(Left3D);
		RightData = GetOrAddBatchData(RightMesh);
		TaskColliders.emplace_back(make_pair(Left3D, RightMesh));
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

	Tasks.push_back(Task);
}

/**
 * @brief Narrow 충돌 판정 중 CPU 기반의 판정을 진행하고, 충돌했다면 적절한 처리 로직을 호출하는 함수
 *
 * @param InLeftObject Object 1
 * @param InRightObject Object 2
 */
void CollisionManager::CheckPair(const CGameObject* InRightObject, const CGameObject* InLeftObject)
{
	// 2D 충돌체 검사
	if (InLeftObject->Collider2D())
	{
		if (InRightObject->Collider2D())
		{
			if (IsCollision(InLeftObject->Collider2D(), InRightObject->Collider2D()))
			{
				AddFrameCollision(InLeftObject->Collider2D(), InRightObject->Collider2D());
			}
		}
	}

	// 3D 충돌체 검사
	if (InLeftObject->Collider3D())
	{
		if (InRightObject->Collider3D())
		{
			if (IsCollision(InLeftObject->Collider3D(), InRightObject->Collider3D()))
			{
				AddFrameCollision(InLeftObject->Collider3D(), InRightObject->Collider3D());
			}
		}

		if (InRightObject->LandScape())
		{
			if (IsCollision(InLeftObject->Collider3D(), InRightObject->LandScape()))
			{
				AddFrameCollision(InLeftObject->Collider3D(), InRightObject->LandScape());
			}
		}
	}

	// LandScape 검사
	if (InLeftObject->LandScape())
	{
		if (InRightObject->Collider3D())
		{
			if (IsCollision(InRightObject->Collider3D(), InLeftObject->LandScape()))
			{
				AddFrameCollision(InLeftObject->LandScape(), InRightObject->Collider3D());
			}
		}
	}

	// LOG_INFO_F("[Collision][Narrow] Object {} & {}, Failed To Match Type Pairing",
	//            WStringToString(PLeftObject->GetName()), WStringToString(PRightObject->GetName()));
}

void CollisionManager::AddFrameCollision(ColliderVariant InLeftCollider, ColliderVariant InRightCollider)
{
	visit([&](auto* LeftCollider, auto* RightCollider)
	{
		FrameCollisionSet.insert(COLLISION_ID(LeftCollider->GetID(), RightCollider->GetID()).ID);
	}, InLeftCollider, InRightCollider);
}

/**********************************/
/** Narrow Collision Check Logic **/
/**********************************/

// TODO(KHJ): 이하의 코드 재검토 필요

bool CollisionManager::IsCollision(const CCollider2D* InLeftCollider, const CCollider2D* InRightCollider)
{
	// SAT(Separating Axis Theorem) 기반 2D 충돌 판정
	static Vec3 arrRect[4] = {
		Vec3(-0.5f, 0.5f, 0.f), Vec3(0.5f, 0.5f, 0.f), Vec3(0.5f, -0.5f, 0.f), Vec3(-0.5f, -0.5f, 0.f)
	};
	Matrix matColLeft = InLeftCollider->GetColliderWorldMatrix();
	Matrix matColRight = InRightCollider->GetColliderWorldMatrix();
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

bool CollisionManager::IsCollision(const CCollider3D* InLeftCollider, const CCollider3D* InRightCollider)
{
	constexpr float EPSILON = 0.0001f;
	static Vec3 arrCube[8] = {
		Vec3(-0.5f, 0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f), Vec3(0.5f, -0.5f, 0.5f), Vec3(-0.5f, -0.5f, 0.5f),
		Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f), Vec3(0.5f, -0.5f, -0.5f), Vec3(-0.5f, -0.5f, -0.5f)
	};
	Matrix matColLeft = InLeftCollider->GetColliderWorldMat();
	Matrix matColRight = InRightCollider->GetColliderWorldMat();
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

bool CollisionManager::IsCollision(const CCollider3D* InLeftCollider, const CLandScape* InRightCollider)
{
	Vec3 ObjectPos = InLeftCollider->Transform()->GetWorldPos();
	Vec3 LandScapePos = InRightCollider->GetWorldPosByLandScape(ObjectPos);
	if (LandScapePos == Vec3(-10000.f, -10000.f, -10000.f) || ObjectPos.y > LandScapePos.y)
		return false;
	return true;
}
