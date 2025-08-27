#include "pch.h"
#include "Engine/System/Public/Manager/CollisionManager.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "Engine/Runtime/Public/Component/Physics/PlaneCollider.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"
#include "Engine/Runtime/Public/Component/Physics/MeshCollider.h"
#include "Engine/Runtime/Public/Component/Physics/SphereCollider.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

/*************************/
/** Constant Structures **/
/*************************/

/**
 * @brief 세부 충돌 검사에 Compute Shader 처리가 필요한지 판별하는 함수
 * @param InLeftCollider Collider 1
 * @param InRightCollider Collider 2
 * @return CS 처리가 필요하다면 True
 */
bool FCollisionManager::IsNeedCSTask(const IColliderBase* InLeftCollider, const IColliderBase* InRightCollider)
{
	const bool IsMeshA = (InLeftCollider->GetColliderType() == EColliderType::MeshCollider);
	const bool IsMeshB = (InRightCollider->GetColliderType() == EColliderType::MeshCollider);
	const bool Is3DColliderA = (InLeftCollider->GetColliderType() == EColliderType::BoxCollider);
	const bool Is3DColliderB = (InRightCollider->GetColliderType() == EColliderType::BoxCollider);
	// const bool IsSphereA = (InLeftCollider->GetColliderType() == EColliderType::SphereCollider);
	// const bool IsSphereB = (InRightCollider->GetColliderType() == EColliderType::SphereCollider);

	if ((IsMeshA && IsMeshB) || (IsMeshA && Is3DColliderB) || (Is3DColliderA && IsMeshB))
	{
		return true;
	}

	// if ((IsSphereA && IsMeshB) || (IsMeshA && IsSphereB))
	// {
	//     return true;
	// }

	return false;
}

/**
 * @brief MeshCollider 정보를 Buffer에 추가하는 함수
 */
FMeshBatchData FCollisionManager::GetOrAddBatchData(const FMeshCollider* InCollider)
{
	// 중복 생성 방지
	const void* ColliderKey = InCollider;

	if (DataCache.contains(ColliderKey))
	{
		return DataCache.at(ColliderKey);
	}

	FMeshBatchData NewData = {};
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

		NewData.TriangleCount = TotalTriangleCount;
	}

	// Cache에 등록
	DataCache[ColliderKey] = NewData;

	return NewData;
}

/**
 * @brief 배치 처리를 실행하고 결과를 처리하는 함수
 */
void FCollisionManager::ExecuteAndProcessCS()
{
	// Early Return
	if (Tasks.empty())
	{
		return;
	}

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
 * @param InLeftCollider Collision Object 1
 * @param InRightCollider Collision Object 2
 */
void FCollisionManager::AddShaderTask(IColliderBase* InLeftCollider, IColliderBase* InRightCollider)
{
	const bool IsMeshColliderA = (InLeftCollider->GetColliderType() == EColliderType::MeshCollider);
	const bool IsMeshColliderB = (InRightCollider->GetColliderType() == EColliderType::MeshCollider);
	const bool Is3DColliderA = (InLeftCollider->GetColliderType() == EColliderType::BoxCollider);
	const bool Is3DColliderB = (InRightCollider->GetColliderType() == EColliderType::BoxCollider);

	FMeshCollider* LeftMesh = static_cast<FMeshCollider*>(InLeftCollider);
	FMeshCollider* RightMesh = static_cast<FMeshCollider*>(InRightCollider);
	FBoxCollider* Left3D = static_cast<FBoxCollider*>(InLeftCollider);
	FBoxCollider* Right3D = static_cast<FBoxCollider*>(InRightCollider);

	FMeshBatchData LeftData, RightData;

	if (IsMeshColliderA && IsMeshColliderB)
	{
		LeftData = GetOrAddBatchData(LeftMesh);
		RightData = GetOrAddBatchData(RightMesh);
		TaskColliders.emplace_back(make_pair(LeftMesh, RightMesh));
	}
	else if (IsMeshColliderA && Is3DColliderB)
	{
		LeftData = GetOrAddBatchData(LeftMesh);
		RightData = GetOrAddBatchData(Right3D);
		TaskColliders.emplace_back(make_pair(LeftMesh, Right3D));
	}
	else if (Is3DColliderA && IsMeshColliderB)
	{
		LeftData = GetOrAddBatchData(Left3D);
		RightData = GetOrAddBatchData(RightMesh);
		TaskColliders.emplace_back(make_pair(Left3D, RightMesh));
	}
	else
	{
		assert(!R"(Shader 처리가 필요 없는 충돌체이므로 여기서 들어올 수 없음)");
		return;
	}

	// CS 작업 생성 및 추가
	tCollisionTask Task = {};
	Task.LeftVertexOffset = LeftData.VertexOffset;
	Task.LeftIndexOffset = LeftData.IndexOffset;
	Task.LeftTriCount = LeftData.TriangleCount;
	Task.RightVertexOffset = RightData.VertexOffset;
	Task.RightIndexOffset = RightData.IndexOffset;
	Task.RightTriCount = RightData.TriangleCount;

	Tasks.push_back(Task);
}

/**
 * @brief Narrow 충돌 판정 중 CPU 기반의 판정을 진행하고, 충돌했다면 적절한 처리 로직을 호출하는 함수
 *
 * @param InLeftCollider Collider 1
 * @param InRightCollider Collider 2
 */
void FCollisionManager::CheckCollisionInCPU(IColliderBase* InLeftCollider, IColliderBase* InRightCollider)
{
	const bool IsLeft2D = (InLeftCollider->GetColliderType() == EColliderType::PlaneCollider);
	const bool IsRight2D = (InRightCollider->GetColliderType() == EColliderType::PlaneCollider);
	const bool IsLeft3D = (InLeftCollider->GetColliderType() == EColliderType::BoxCollider);
	const bool IsRight3D = (InRightCollider->GetColliderType() == EColliderType::BoxCollider);
	const bool IsLeftSphere = (InLeftCollider->GetColliderType() == EColliderType::SphereCollider);
	const bool IsRightSphere = (InRightCollider->GetColliderType() == EColliderType::SphereCollider);
	const bool IsLeftMesh = (InLeftCollider->GetColliderType() == EColliderType::MeshCollider);
	const bool IsRightMesh = (InRightCollider->GetColliderType() == EColliderType::MeshCollider);

	FPlaneCollider* Left2D = static_cast<FPlaneCollider*>(InLeftCollider);
	FPlaneCollider* Right2D = static_cast<FPlaneCollider*>(InRightCollider);
	FBoxCollider* LeftBox = static_cast<FBoxCollider*>(InLeftCollider);
	FBoxCollider* RightBox = static_cast<FBoxCollider*>(InRightCollider);
	FSphereCollider* LeftSphere = static_cast<FSphereCollider*>(InLeftCollider);
	FSphereCollider* RightSphere = static_cast<FSphereCollider*>(InRightCollider);
	FMeshCollider* LeftMesh = static_cast<FMeshCollider*>(InLeftCollider);
	FMeshCollider* RightMesh = static_cast<FMeshCollider*>(InRightCollider);

	// 2D 충돌체 검사
	if (IsLeft2D && IsRight2D && IsCollision(Left2D, Right2D))
	{
		AddFrameCollision(Left2D, Right2D);
	}

	// 3D 충돌체 검사 (Box vs Box)
	else if (IsLeft3D && IsRight3D && IsCollision(LeftBox, RightBox))
	{
		AddFrameCollision(LeftBox, RightBox);
	}

	// Sphere 충돌체 검사 (Sphere vs Sphere)
	else if (IsLeftSphere && IsRightSphere && IsCollision(LeftSphere, RightSphere))
	{
		AddFrameCollision(LeftSphere, RightSphere);
	}

	// Sphere vs Box 충돌체 검사
	else if (IsLeftSphere && IsRight3D && IsCollision(LeftSphere, RightBox))
	{
		AddFrameCollision(LeftSphere, RightBox);
	}

	// Box vs Sphere 충돌체 검사
	else if (IsLeft3D && IsRightSphere && IsCollision(LeftBox, RightSphere))
	{
		AddFrameCollision(LeftBox, RightSphere);
	}

	// Sphere vs Mesh 충돌체 검사
	else if (IsLeftSphere && IsRightMesh && IsCollision(LeftSphere, RightMesh))
	{
		AddFrameCollision(LeftSphere, RightMesh);
	}

	// Mesh vs Sphere 충돌체 검사
	else if (IsLeftMesh && IsRightSphere && IsCollision(LeftMesh, RightSphere))
	{
		AddFrameCollision(LeftMesh, RightSphere);
	}

	// LOG_INFO_F("[Collision][Narrow] Object {} & {}, Failed To Match Type Pairing",
	//            WStringToString(PLeftObject->GetName()), WStringToString(PRightObject->GetName()));
}

/**********************************/
/** Narrow Collision Check Logic **/
/**********************************/

// TODO(KHJ): 코드 재검토 필요
bool FCollisionManager::IsCollision(const FPlaneCollider* InLeftCollider, const FPlaneCollider* InRightCollider)
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

// TODO(KHJ): 코드 재검토 필요
bool FCollisionManager::IsCollision(const FBoxCollider* InLeftCollider, const FBoxCollider* InRightCollider)
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

/**
 * @brief Sphere 간의 충돌 판정을 처리하는 함수
 * 구의 중심 간의 거리를 측정, 반지름을 더한 값보다 거리가 짧으면 충돌 상태이다
 */
bool FCollisionManager::IsCollision(const FSphereCollider* InLeftCollider, const FSphereCollider* InRightCollider)
{
	Vec3 CenterA = InLeftCollider->GetWorldCenter();
	Vec3 CenterB = InRightCollider->GetWorldCenter();
	float RadiusA = InLeftCollider->GetWorldRadius();
	float RadiusB = InRightCollider->GetWorldRadius();

	float Distance = (CenterA - CenterB).Length();
	float CombinedRadius = RadiusA + RadiusB;

	return Distance <= CombinedRadius;
}

/**
 * @brief Sphere vs Box 충돌 검사
 * Sphere의 중심을 Box Local로 가져가서 가장 가까운 지점을 찾고, 해당 지점을 World 변환하여 거리 측정한 값을 기반으로 판정한다
 */
bool FCollisionManager::IsCollision(const FSphereCollider* InSphereCollider, const FBoxCollider* InBoxCollider)
{
	// Sphere Data
	Vec3 SphereCenter = InSphereCollider->GetWorldCenter();
	float SphereRadius = InSphereCollider->GetWorldRadius();

	// Sphere Center To Box Local
	Matrix BoxWorldMatrix = InBoxCollider->GetColliderWorldMat();
	Matrix InverseBoxMatrix;
	XMStoreFloat4x4(&InverseBoxMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&BoxWorldMatrix)));
	Vec3 SphereCenterLocal = XMVector3TransformCoord(SphereCenter, InverseBoxMatrix);

	// Get Local Closest Point
	Vec3 ClosestPoint;
	ClosestPoint.x = max(-0.5f, min(0.5f, SphereCenterLocal.x));
	ClosestPoint.y = max(-0.5f, min(0.5f, SphereCenterLocal.y));
	ClosestPoint.z = max(-0.5f, min(0.5f, SphereCenterLocal.z));

	// Find World Closest Point
	Vec3 WorldClosest = XMVector3TransformCoord(ClosestPoint, BoxWorldMatrix);
	float WorldDistance = (SphereCenter - WorldClosest).Length();

	return WorldDistance <= SphereRadius;
}

/**
 * @brief Box vs Sphere 충돌 검사
 * param swap function return
 */
bool FCollisionManager::IsCollision(const FBoxCollider* InBoxCollider, const FSphereCollider* InSphereCollider)
{
	return IsCollision(InSphereCollider, InBoxCollider);
}

/**
 * @brief Sphere vs Mesh 충돌 검사
 * Sphere와 Mesh의 맨 가까운 삼각형과의 거리를 계산하여 충돌 검사
 */
bool FCollisionManager::IsCollision(const FSphereCollider* InSphereCollider, const FMeshCollider* InMeshCollider)
{
	// Sphere Data
	Vec3 SphereCenter = InSphereCollider->GetWorldCenter();
	float SphereRadius = InSphereCollider->GetWorldRadius();

	// Mesh 정보
	Ptr<CMesh> Mesh = InMeshCollider->GetMesh();
	if (!Mesh.Get())
	{
		return false;
	}

	// World Matrix 가져오기
	Matrix WorldMatrix = InMeshCollider->GetOwner()->Transform()->GetWorldMat();
	Vtx* Vertices = static_cast<Vtx*>(Mesh->GetVtxSysMem());

	// 모든 삼각형에 대해 검사
	for (UINT i = 0; i < Mesh->GetSubsetCount(); ++i)
	{
		const auto& Subset = Mesh->GetIndexInfo()[i];
		UINT* Indices = static_cast<UINT*>(Subset.IdxSysMem);

		for (UINT j = 0; j < Subset.IdxCount; j += 3)
		{
			UINT Idx0 = Indices[j + 0];
			UINT Idx1 = Indices[j + 1];
			UINT Idx2 = Indices[j + 2];

			// World Vertex
			Vec3 Vertex0 = XMVector3TransformCoord(Vertices[Idx0].vPos, WorldMatrix);
			Vec3 Vertex1 = XMVector3TransformCoord(Vertices[Idx1].vPos, WorldMatrix);
			Vec3 Vertex2 = XMVector3TransformCoord(Vertices[Idx2].vPos, WorldMatrix);

			// 삼각형과 Sphere 중심 간의 가장 가까운 점 찾기
			Vec3 ClosestPoint = GetClosestPointOnTriangle(SphereCenter, Vertex0, Vertex1, Vertex2);
			float DistanceSquared = (SphereCenter - ClosestPoint).LengthSquared();

			// 충돌 판정 & Early Return
			if (DistanceSquared <= pow(SphereRadius, 2))
			{
				return true;
			}
		}
	}

	// 모든 거리가 반지름보다 먼 케이스
	return false;
}

/**
 * @brief Mesh vs Sphere 충돌 검사
 * param swap function return
 */
bool FCollisionManager::IsCollision(const FMeshCollider* InMeshCollider, const FSphereCollider* InSphereCollider)
{
	return IsCollision(InSphereCollider, InMeshCollider);
}

/**
 * @brief 3D 공간의 한 점(InPoint)에서 삼각형(A, B, C)까지의 가장 가까운 점을 찾습니다.
 * @param InPoint 기준이 되는 3D 공간의 점 P
 * @param InVertexA 삼각형 꼭짓점 A
 * @param InVertexB 삼각형 꼭짓점 B
 * @param InVertexC 삼각형 꼭짓점 C
 * @return 삼각형 위에서 InPoint와 가장 가까운 점의 좌표
 */
Vec3 FCollisionManager::GetClosestPointOnTriangle(const Vec3& InPoint, const Vec3& InVertexA, const Vec3& InVertexB,
                                                  const Vec3& InVertexC)
{
	Vec3 AB = InVertexB - InVertexA;
	Vec3 AC = InVertexC - InVertexA;
	Vec3 AP = InPoint - InVertexA;

	// 점 P와 꼭짓점 A의 관계 계산
	float d1 = AB.Dot(AP);
	float d2 = AC.Dot(AP);

	// Area 1: 꼭짓점 A가 가장 가까운 경우
	// d1과 d2가 모두 0 이하면, 점 P는 AB, AC 방향의 반대편에 위치
	// 이 영역에서는 꼭짓점 A가 무조건 가장 가까운 상황
	if (d1 <= 0.f && d2 <= 0.f)
	{
		return InVertexA; // 무게중심 좌표 (1, 0, 0)에 해당
	}

	// 점 P와 꼭짓점 B의 관계 계산
	Vec3 BP = InPoint - InVertexB;
	float d3 = AB.Dot(BP);
	float d4 = AC.Dot(BP);

	// Area 2: 꼭짓점 B가 가장 가까운 경우
	// d3 >= 0: P가 AB를 기준으로 B보다 앞에 있음
	// d4 <= d3: P가 변 BC보다는 꼭짓점 B에 더 가까운 영역에 있음을 보장
	if (d3 >= 0.f && d4 <= d3)
	{
		return InVertexB; // 무게중심 좌표 (0, 1, 0)에 해당
	}

	// 변 AB 영역 확인 By CCW Algorithm
	// vc는 P가 AB 변의 안쪽 혹은 바깥쪽에 위치하는지 판별할 수 있는 값 (무게중심 좌표와 관련)
	float vc = d1 * d4 - d3 * d2;

	// Area 3: 변 AB 위의 한 점이 가장 가까운 경우
	// vc <= 0: P가 변 AB의 바깥쪽에 있음
	// d1 >= 0: P가 AB 기준 A보다 앞에 있음
	// d3 <= 0: P가 AB 기준 B보다 뒤에 있음
	// 세 조건을 모두 만족하면, P를 변 AB에 수직으로 내린 점이 A와 B 사이에 존재
	if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
	{
		float v = d1 / (d1 - d3);
		return InVertexA + v * AB; // 무게중심 좌표 (1-v, v, 0)
	}

	// 점 P와 꼭짓점 C의 관계 계산
	Vec3 CP = InPoint - InVertexC;
	float d5 = AB.Dot(CP);
	float d6 = AC.Dot(CP);

	// Area 4: 꼭짓점 C가 가장 가까운 경우
	// d6 >= 0: P가 AC를 기준으로 C보다 앞에 있음
	// d5 <= d6: P가 변 BC보다는 꼭짓점 C에 더 가까운 영역에 있음을 보장
	if (d6 >= 0.f && d5 <= d6)
	{
		return InVertexC; // 무게중심 좌표 (0, 0, 1)에 해당
	}

	// 변 AC 영역 확인 By CCW Algorithm
	// vb는 P가 AC 변의 안쪽 혹은 바깥쪽에 위치하는지 판별할 수 있는 값
	float vb = d5 * d2 - d1 * d6;

	// Area 5: 변 AC 위의 한 점이 가장 가까운 경우
	// vb <= 0: P가 변 AC의 바깥쪽에 있음
	// d2 >= 0: P가 AC 기준 A보다 앞에 있음
	// d6 <= 0: P가 AC 기준 C보다 뒤에 있음
	// 세 조건을 만족하면, P를 변 AC에 수직으로 내린 점이 A와 C 사이에 존재
	if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
	{
		float w = d2 / (d2 - d6);
		return InVertexA + w * AC; // 무게중심 좌표 (1-w, 0, w)
	}

	// 변 BC 영역 확인 By CCW Algorithm
	// va는 P가 BC 변의 안쪽 혹은 바깥쪽에 위치하는지 판별할 수 있는 값
	float va = d3 * d6 - d5 * d4;

	// Area 6: 변 BC 위의 한 점이 가장 가까운 경우
	// va <= 0: P가 변 BC의 바깥쪽에 있음
	// (d4 - d3) >= 0 & (d5 - d6) >= 0: P를 변 BC에 내린 점이 B와 C 사이에 있음을 보장
	if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return InVertexB + w * (InVertexC - InVertexB); // 무게중심 좌표 (0, 1-w, w)
	}

	// Area 7: 삼각형 내부 면 위의 한 점이 가장 가까운 경우
	// 위의 6개 영역 어디에도 속하지 않으면, P는 삼각형 면의 수직 위에 있는 존재
	// P를 삼각형 평면에 그대로 투영한 점이 가장 가까운 점
	// va, vb, vc는 삼각형의 무게중심 좌표와 비례
	float denom = 1.f / (va + vb + vc);
	// 꼭짓점 A에 대한 가중치는 1 - v - w
	float v = vb * denom; // 꼭짓점 B에 대한 가중치
	float w = vc * denom; // 꼭짓점 C에 대한 가중치

	// 최종 좌표 = A + v * (B-A) + w * (C-A)
	return InVertexA + AB * v + AC * w;
}

// bool CollisionManager::IsCollision(const FBoxCollider* InLeftCollider, const FLandScape* InRightCollider)
// {
// 	Vec3 ObjectPos = InLeftCollider->Transform()->GetWorldPos();
// 	Vec3 LandScapePos = InRightCollider->GetWorldPosByLandScape(ObjectPos);
// 	if (LandScapePos == Vec3(-10000.f, -10000.f, -10000.f) || ObjectPos.y > LandScapePos.y)
// 		return false;
// 	return true;
// }
