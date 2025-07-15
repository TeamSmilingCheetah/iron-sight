#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider2D.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"

/******************/
/** Narrow Check **/
/******************/

/**
 * @brief 충돌 가능성이 있는 오브젝트 쌍에 대해 충돌 판정을 처리하는 함수
 */
void FCollisionManager::CheckNarrowPhase()
{
	for (auto& Pair : MCandidatePairVector)
	{
		CheckPairNarrow(Pair.first, Pair.second);
	}
}

/**
 * @brief Narrow 충돌 판정을 진행하고, 충돌했다면 적절한 처리 로직을 호출하는 함수
 *
 * @param PLeftObject Object 1
 * @param PRightObject Object 2
 */
void FCollisionManager::CheckPairNarrow(CGameObject* PRightObject, CGameObject* PLeftObject)
{
	// 1. 2D 충돌체 검사
	if (PLeftObject->Collider2D())
	{
		if (PRightObject->Collider2D())
		{
			if (IsCollision(PLeftObject->Collider2D(), PRightObject->Collider2D()))
			{
				ProcessCollision(PLeftObject->Collider2D(), PRightObject->Collider2D());
			}
			return;
		}
	}

	// 2. 3D 충돌체 검사
	if (PLeftObject->Collider3D())
	{
		if (PRightObject->Collider3D())
		{
			if (IsCollision(PLeftObject->Collider3D(), PRightObject->Collider3D()))
			{
				ProcessCollision(PLeftObject->Collider3D(), PRightObject->Collider3D());
			}
			return;
		}

		if (PRightObject->LandScape())
		{
			if (IsCollision(PLeftObject->Collider3D(), PRightObject->LandScape()))
			{
				ProcessCollision(PLeftObject->Collider3D(), PRightObject->LandScape());
			}
			return;
		}

		if (PRightObject->ColliderRay())
		{
			if (IsCollision(PLeftObject->Collider3D(), PRightObject->ColliderRay()))
			{
				ProcessCollision(PRightObject->ColliderRay(), PLeftObject->Collider3D());
			}
			return;
		}

		if (PRightObject->MeshCollider())
		{
			if (IsCollision(PLeftObject->Collider3D(), PRightObject->MeshCollider()))
			{
				ProcessCollision(PLeftObject->Collider3D(), PRightObject->MeshCollider());
			}
			return;
		}
	}

	// 3. LandScape 검사
	if (PLeftObject->LandScape())
	{
		// 3D간 충돌의 경우
		if (PRightObject->Collider3D())
		{
			if (IsCollision(PRightObject->Collider3D(), PLeftObject->LandScape()))
			{
				ProcessCollision(PLeftObject->LandScape(), PRightObject->Collider3D());
			}
			return;
		}

		// Ray와 충돌의 경우
		if (PRightObject->ColliderRay())
		{
			if (IsCollision(PLeftObject->LandScape(), PRightObject->ColliderRay()))
			{
				ProcessCollision(PLeftObject->LandScape(), PRightObject->ColliderRay());
			}
			return;
		}
	}

	// 4. RayCast 검사
	if (PLeftObject->ColliderRay())
	{
		if (PRightObject->Collider3D())
		{
			if (IsCollision(PRightObject->Collider3D(), PLeftObject->ColliderRay()))
			{
				ProcessCollision(PLeftObject->ColliderRay(), PRightObject->Collider3D());
			}
			return;
		}

		// LandScape와 충돌의 경우
		if (PRightObject->LandScape())
		{
			if (IsCollision(PRightObject->LandScape(), PLeftObject->ColliderRay()))
			{
				ProcessCollision(PLeftObject->ColliderRay(), PRightObject->LandScape());
			}
			return;
		}
	}

	// 5. Mesh Collider
	if (PLeftObject->MeshCollider())
	{
		if (PRightObject->MeshCollider())
		{
			if (IsCollision(PLeftObject->MeshCollider(), PRightObject->MeshCollider()))
			{
				ProcessCollision(PLeftObject->MeshCollider(), PRightObject->MeshCollider());
			}
			return;
		}

		if (PRightObject->Collider3D())
		{
			if (IsCollision(PLeftObject->MeshCollider(), PRightObject->Collider3D()))
			{
				ProcessCollision(PLeftObject->MeshCollider(), PRightObject->Collider3D());
			}
			return;
		}
	}
}

/**********************************/
/** Narrow Collision Check Logic **/
/**********************************/

bool FCollisionManager::IsCollision(const CCollider2D* left, const CCollider2D* right)
{
    // SAT(Separating Axis Theorem) 기반 2D 충돌 판정
    static Vec3 arrRect[4] = {
        Vec3(-0.5f, 0.5f, 0.f), Vec3(0.5f, 0.5f, 0.f), Vec3(0.5f, -0.5f, 0.f), Vec3(-0.5f, -0.5f, 0.f)
    };
    Matrix matColLeft = left->GetColliderWorldMatrix();
    Matrix matColRight = right->GetColliderWorldMatrix();
    Vec3 arrProj[4] = {};
    arrProj[0] = XMVector3TransformCoord(arrRect[1], matColLeft) - XMVector3TransformCoord(arrRect[0], matColLeft);
    arrProj[1] = XMVector3TransformCoord(arrRect[3], matColLeft) - XMVector3TransformCoord(arrRect[0], matColLeft);
    arrProj[2] = XMVector3TransformCoord(arrRect[1], matColRight) - XMVector3TransformCoord(arrRect[0], matColRight);
    arrProj[3] = XMVector3TransformCoord(arrRect[3], matColRight) - XMVector3TransformCoord(arrRect[0], matColRight);
    Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColLeft) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColRight);
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

bool FCollisionManager::IsCollision(CCollider3D* PLeftCollider, CCollider3D* PRightCollider)
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
    for (int i = 0; i < 8; i++) {
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
    for (int i = 0; i < 3; i++) {
        leftAxis[i].Normalize();
        rightAxis[i].Normalize();
        axes[axisCount++] = leftAxis[i];
        axes[axisCount++] = rightAxis[i];
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Vec3 crossAxis = leftAxis[i].Cross(rightAxis[j]);
            if (crossAxis.Length() > EPSILON) {
                crossAxis.Normalize();
                axes[axisCount++] = crossAxis;
            }
        }
    }
    for (int a = 0; a < axisCount; ++a) {
        float leftMin = FLT_MAX, leftMax = -FLT_MAX, rightMin = FLT_MAX, rightMax = -FLT_MAX;
        for (int i = 0; i < 8; i++) {
            float dot = leftVertices[i].Dot(axes[a]);
            leftMin = min(leftMin, dot);
            leftMax = max(leftMax, dot);
        }
        for (int i = 0; i < 8; i++) {
            float dot = rightVertices[i].Dot(axes[a]);
            rightMin = min(rightMin, dot);
            rightMax = max(rightMax, dot);
        }
        if (leftMin > rightMax + EPSILON || rightMin > leftMax + EPSILON)
            return false;
    }
    return true;
}

bool FCollisionManager::IsCollision(const CCollider3D* box, const CLandScape* land)
{
    Vec3 ObjectPos = box->Transform()->GetWorldPos();
    Vec3 LandScapePos = land->GetWorldPosByLandScape(ObjectPos);
    if (LandScapePos == Vec3(-10000.f, -10000.f, -10000.f) || ObjectPos.y > LandScapePos.y)
        return false;
    return true;
}

bool FCollisionManager::IsCollision(CCollider3D* PLeftCollider, CColliderRay* PRightCollider)
{
    static Vec3 arrCube[8] = {
        Vec3(-0.5f, 0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f), Vec3(0.5f, -0.5f, 0.5f), Vec3(-0.5f, -0.5f, 0.5f),
        Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f), Vec3(0.5f, -0.5f, -0.5f), Vec3(-0.5f, -0.5f, -0.5f)
    };
    Matrix matWorld = PLeftCollider->GetColliderWorldMat();
    Vec3 worldVerts[8];
    for (int i = 0; i < 8; ++i)
        worldVerts[i] = XMVector3TransformCoord(arrCube[i], matWorld);
    static int triangles[12][3] = {
        {0, 2, 1}, {0, 3, 2}, {1, 5, 6}, {1, 6, 2},
        {4, 5, 6}, {4, 6, 7}, {0, 4, 7}, {0, 7, 3},
        {4, 5, 1}, {4, 1, 0}, {3, 2, 6}, {3, 6, 7}
    };
    Vec3 rayPos = PRightCollider->GetRayFinalPos();
    Vec3 rayDir = PRightCollider->GetRayFinalDir();
    float rayMaxDist = PRightCollider->GetRayLength();
    float closestDist = FLT_MAX;
    bool hasCollision = false;
    for (int i = 0; i < 12; ++i) {
        Vec3* triVerts[3] = {
            &worldVerts[triangles[i][0]],
            &worldVerts[triangles[i][1]],
            &worldVerts[triangles[i][2]]
        };
        Vec3 crossPos = Vec3(0.f);
        float dist = 0;
        Vec3 edge1 = *triVerts[1] - *triVerts[0];
        Vec3 edge2 = *triVerts[2] - *triVerts[0];
        Vec3 triangleNormal = edge1.Cross(edge2);
        triangleNormal.Normalize();
        if (IntersectsRay(triVerts, rayPos, rayDir, crossPos, dist)) {
            if (dist >= 0.f && dist <= rayMaxDist && dist < closestDist) {
                closestDist = dist;
                hasCollision = true;
            }
        }
    }
    return hasCollision;
}

bool FCollisionManager::IsCollision(CLandScape* PRightCollider, CColliderRay* PLeftCollider)
{
    tRayCollision RayInfo;
    RayInfo.RayObj = reinterpret_cast<void*>(PLeftCollider);
    RayInfo.RayWorldPos = PLeftCollider->GetRayFinalPos();
    RayInfo.RayDir = PLeftCollider->GetRayFinalDir();
    RayInfo.RayLength = PLeftCollider->GetRayLength();
    PRightCollider->AddRayCol(RayInfo);

	return false; // 실제 충돌 여부는 LandScape 내부에서 처리됨
}

/**
 * @brief 실제 메시 충돌체끼리 충돌했는지 여부를 세부적으로 확인하는 함수
 *
 * @param PLeftCollider Mesh Collider 1
 * @param PRightCollider Mesh Collider 2
 * @return 충돌 여부
 */
bool FCollisionManager::IsCollision(CMeshCollider* PLeftCollider, CMeshCollider* PRightCollider)
{
	// 1. Variable Setting
	Ptr<CMesh> LeftMesh = PLeftCollider->GetMesh();
	Ptr<CMesh> RightMesh = PRightCollider->GetMesh();

	// Early Return
	if (!LeftMesh.Get() || !RightMesh.Get())
	{
		return false;
	}

	UINT LeftVtxCount = LeftMesh->GetVertexCount();
	UINT RightVtxCount = RightMesh->GetVertexCount();

	// Early Return
	if (!LeftVtxCount || !RightVtxCount)
	{
		return false;
	}

	// 2. Triangle Counting
	INT32 LeftTriCount = 0;
	INT32 RightTriCount = 0;

	for (UINT i = 0; i < LeftMesh->GetSubsetCount(); ++i)
	{
		INT32 LeftIdxCount = LeftMesh->GetIndexInfo()[i].IdxCount;
		LeftTriCount += LeftIdxCount / 3;
	}
	for (UINT i = 0; i < RightMesh->GetSubsetCount(); ++i)
	{
		INT32 RightIdxCount = RightMesh->GetIndexInfo()[i].IdxCount;
		RightTriCount += RightIdxCount / 3;
	}

	// Early Return
	if (!LeftTriCount || !RightTriCount)
		return false;

	// 3. Prepare Buffer
	vector<SimpleVtx> LeftVtxVector(LeftVtxCount);
	vector<SimpleVtx> RightVtxVector(RightVtxCount);
	Vtx* LeftVtxArr = static_cast<Vtx*>(LeftMesh->GetVtxSysMem());
	Vtx* RightVtxArr = static_cast<Vtx*>(RightMesh->GetVtxSysMem());
	for (UINT i = 0; i < LeftVtxCount; ++i)
	{
		LeftVtxVector[i].pos = LeftVtxArr[i].vPos;
	}
	for (UINT i = 0; i < RightVtxCount; ++i)
	{
		RightVtxVector[i].pos = RightVtxArr[i].vPos;
	}

	vector<SimpleIdx> LeftIdxVector(LeftTriCount);
	UINT TotalLeftIdx = 0;
	for (UINT i = 0; i < LeftMesh->GetSubsetCount(); ++i)
	{
		UINT* LeftIdxArr = static_cast<UINT*>(LeftMesh->GetIndexInfo()[i].IdxSysMem);
		for (size_t j = 0; j < LeftMesh->GetIndexInfo()[i].IdxCount / 3; ++j)
		{
			LeftIdxVector[TotalLeftIdx] = {LeftIdxArr[j * 3], LeftIdxArr[j * 3 + 1], LeftIdxArr[j * 3 + 2]};
			++TotalLeftIdx;
		}
	}

	vector<SimpleIdx> RightIdxVector(RightTriCount);
	UINT TotalRightIdx = 0;
	for (UINT i = 0; i < RightMesh->GetSubsetCount(); ++i)
	{
		UINT* RightIdxArr = static_cast<UINT*>(RightMesh->GetIndexInfo()[i].IdxSysMem);
		for (size_t j = 0; j < RightMesh->GetIndexInfo()[i].IdxCount / 3; ++j)
		{
			RightIdxVector[TotalRightIdx] = {RightIdxArr[j * 3], RightIdxArr[j * 3 + 1], RightIdxArr[j * 3 + 2]};
			++TotalRightIdx;
		}
	}

	// 4. Create StructuredBuffer
	CStructuredBuffer LeftVtxBuffer, RightVtxBuffer, LeftIdxBuffer, RightIdxBuffer;
	LeftVtxBuffer.Create(sizeof(SimpleVtx), LeftVtxCount, SB_TYPE::SRV_ONLY, true, LeftVtxVector.data());
	RightVtxBuffer.Create(sizeof(SimpleVtx), RightVtxCount, SB_TYPE::SRV_ONLY, true, RightVtxVector.data());
	LeftIdxBuffer.Create(sizeof(SimpleIdx), LeftTriCount, SB_TYPE::SRV_ONLY, true, LeftIdxVector.data());
	RightIdxBuffer.Create(sizeof(SimpleIdx), RightTriCount, SB_TYPE::SRV_ONLY, true, RightIdxVector.data());

	CStructuredBuffer CountBuffer, OutputBuffer;

	UINT MaxCollision = 4096;
	vector<UINT> CountArr(1);
	vector<CollisionResult> CollisionArr(MaxCollision);
	CountArr[0] = 0;

	CountBuffer.Create(sizeof(UINT), 1, SB_TYPE::SRV_UAV, true, CountArr.data());
	OutputBuffer.Create(sizeof(CollisionResult), MaxCollision, SB_TYPE::SRV_UAV, true, CollisionArr.data());

	// 5. ComputeShader Execute
	MMeshCollisionCS.SetLeftVertices(&LeftVtxBuffer);
	MMeshCollisionCS.SetLeftIndices(&LeftIdxBuffer);
	MMeshCollisionCS.SetRightVertices(&RightVtxBuffer);
	MMeshCollisionCS.SetRightIndices(&RightIdxBuffer);
	MMeshCollisionCS.SetCount(&CountBuffer);
	MMeshCollisionCS.SetResults(&OutputBuffer);
	MMeshCollisionCS.SetTriCounts(LeftTriCount, RightTriCount);
	MMeshCollisionCS.Execute();

	// 6. Result Check
	CountBuffer.GetData(CountArr.data());
	OutputBuffer.GetData(CollisionArr.data());

	static int CollisionCount = CountArr[0];

	if (CollisionCount)
	{
		// Calculate Average Normal with Penetration Depth
		Vec3 LeftAverageNormal = {0, 0, 0};
		Vec3 RightAverageNormal = {0, 0, 0};
		float MaxPenetrationDepth;

		for (int i = 0; i < CollisionCount; ++i)
		{
			Vec3 LeftNormal = CollisionArr[i].LeftNormal;
			Vec3 RightNormal = CollisionArr[i].RightNormal;
			float CurrentPenetration = CollisionArr[i].PenetrationDepth;

			LeftAverageNormal += LeftNormal;
			RightAverageNormal += RightNormal;

			// Update Max Depth
			MaxPenetrationDepth = max(MaxPenetrationDepth, min(CurrentPenetration, 1.0f));
		}

		// Calculate Collision Normal
		LeftAverageNormal = LeftAverageNormal / static_cast<float>(CollisionCount);
		RightAverageNormal = RightAverageNormal / static_cast<float>(CollisionCount);
		LeftAverageNormal.Normalize();
		RightAverageNormal.Normalize();

		// Add Safety Margin
		constexpr float SafetyMargin = 0.02f;
		MaxPenetrationDepth = max(0.0f, MaxPenetrationDepth - SafetyMargin);

		// Set Normal & Penetration Depth
		PLeftCollider->SetCollisionNormal(LeftAverageNormal);
		PLeftCollider->SetPenetrationDepth(MaxPenetrationDepth);
		PRightCollider->SetCollisionNormal(RightAverageNormal);
		PRightCollider->SetPenetrationDepth(MaxPenetrationDepth);

		return true;
	}

	return false;
}

/**
 * @brief 메시 충돌체의 각 정점 중 하나라도 3D 충돌체와 충돌했는지 판단하는 함수
 *
 * @param PMeshCollider Mesh Collider
 * @param P3DCollider 3D Collider
 * @return 충돌 여부
 */
bool FCollisionManager::IsCollision(CMeshCollider* PMeshCollider,  CCollider3D* P3DCollider)
{
	// 1. Variable Setting
	Ptr<CMesh> Mesh = PMeshCollider->GetMesh();

	// Early Return
	if (!Mesh.Get())
	{
		return false;
	}

	UINT VtxCount = Mesh->GetVertexCount();

	// Early Return
	if (!VtxCount)
	{
		return false;
	}

	// 2. Triangle Counting
	INT32 TriCount = 0;
	for (UINT i = 0; i < Mesh->GetSubsetCount(); ++i)
	{
		TriCount += Mesh->GetIndexInfo()[i].IdxCount / 3;
	}

	// Early Return
	if (!TriCount)
	{
		return false;
	}

	// 3. Prepare Buffer
	Matrix WorldMatrix = PMeshCollider->GetOwner()->Transform()->GetWorldMat();

	vector<SimpleVtx> VtxVector(VtxCount);
	Vtx* VtxArr = static_cast<Vtx*>(Mesh->GetVtxSysMem());
	for (UINT i = 0; i < VtxCount; ++i)
	{
		VtxVector[i].pos = XMVector3TransformCoord(VtxArr[i].vPos, WorldMatrix);
	}

	vector<SimpleIdx> IdxVector(TriCount);
	UINT TotalIdx = 0;
	for (UINT i = 0; i < Mesh->GetSubsetCount(); ++i)
	{
		UINT* IdxArr = static_cast<UINT*>(Mesh->GetIndexInfo()[i].IdxSysMem);
		for (size_t j = 0; j < Mesh->GetIndexInfo()[i].IdxCount / 3; ++j)
		{
			IdxVector[TotalIdx] = {IdxArr[j * 3], IdxArr[j * 3 + 1], IdxArr[j * 3 + 2]};
			++TotalIdx;
		}
	}

	// Prepare 3D Collider Buffer
	vector<SimpleVtx> AABBVtxVector(8);
	vector<SimpleIdx> AABBIdxVector(12);

	Vec3 CubeArr[8] = {
		Vec3(-0.5f, 0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f),
		Vec3(0.5f, -0.5f, 0.5f), Vec3(-0.5f, -0.5f, 0.5f),
		Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f),
		Vec3(0.5f, -0.5f, -0.5f), Vec3(-0.5f, -0.5f, -0.5f)
	};

	// 3D Collider Vertex
	for (int i = 0; i < 8; ++i)
	{
		AABBVtxVector[i].pos = XMVector3TransformCoord(CubeArr[i], P3DCollider->GetColliderWorldMat());
	}

	// 3D Collider Triangle Index
	uint32_t Triangles[12][3] = {
		{0, 2, 1}, {0, 3, 2}, {1, 5, 6}, {1, 6, 2},
		{4, 5, 6}, {4, 6, 7}, {0, 4, 7}, {0, 7, 3},
		{4, 5, 1}, {4, 1, 0}, {3, 2, 6}, {3, 6, 7}
	};

	for (int i = 0; i < 12; ++i)
	{
		AABBIdxVector[i] = {Triangles[i][0], Triangles[i][1], Triangles[i][2]};
	}

	// 4. Create StructuredBuffer
	CStructuredBuffer MeshVtxBuffer, MeshIdxBuffer, AABBVtxBuffer, AABBIdxBuffer;
	MeshVtxBuffer.Create(sizeof(SimpleVtx), VtxCount, SB_TYPE::SRV_ONLY, true, VtxVector.data());
	MeshIdxBuffer.Create(sizeof(SimpleIdx), TriCount, SB_TYPE::SRV_ONLY, true, IdxVector.data());
	AABBVtxBuffer.Create(sizeof(SimpleVtx), 8, SB_TYPE::SRV_ONLY, true, AABBVtxVector.data());
	AABBIdxBuffer.Create(sizeof(SimpleIdx), 12, SB_TYPE::SRV_ONLY, true, AABBIdxVector.data());

	CStructuredBuffer CountBuffer, OutputBuffer;

	UINT MaxCollision = 4096;
	vector<UINT> CountArr(1, 0);
	vector<CollisionResult> CollisionArr(MaxCollision);

	CountBuffer.Create(sizeof(UINT), 1, SB_TYPE::SRV_UAV, true, CountArr.data());
	OutputBuffer.Create(sizeof(CollisionResult), MaxCollision, SB_TYPE::SRV_UAV, true, CollisionArr.data());

	// 5. ComputeShader Execute
	MMeshCollisionCS.SetLeftVertices(&MeshVtxBuffer);
	MMeshCollisionCS.SetLeftIndices(&MeshIdxBuffer);
	MMeshCollisionCS.SetRightVertices(&AABBVtxBuffer);
	MMeshCollisionCS.SetRightIndices(&AABBIdxBuffer);
	MMeshCollisionCS.SetCount(&CountBuffer);
	MMeshCollisionCS.SetResults(&OutputBuffer);
	MMeshCollisionCS.SetTriCounts(TriCount, 12);
	MMeshCollisionCS.Execute();

	// 6. Result Check
	CountBuffer.GetData(CountArr.data());
	OutputBuffer.GetData(CollisionArr.data());

	int CollisionCount = CountArr[0];

	if (CollisionCount)
	{
		// Calculate Average Normal with Penetration Depth
		Vec3 LeftAverageNormal = {0, 0, 0};
		Vec3 RightAverageNormal = {0, 0, 0};
		float MaxPenetrationDepth;

		for (int i = 0; i < CollisionCount; ++i)
		{
			Vec3 LeftNormal = CollisionArr[i].LeftNormal;
			Vec3 RightNormal = CollisionArr[i].RightNormal;
			float CurrentPenetration = CollisionArr[i].PenetrationDepth;

			LeftAverageNormal += LeftNormal;
			RightAverageNormal += RightNormal;

			// Update Max Depth
			MaxPenetrationDepth = max(MaxPenetrationDepth, min(CurrentPenetration, 1.0f));
		}

		// Calculate Collision Normal
		LeftAverageNormal = LeftAverageNormal / static_cast<float>(CollisionCount);
		RightAverageNormal = RightAverageNormal / static_cast<float>(CollisionCount);
		LeftAverageNormal.Normalize();
		RightAverageNormal.Normalize();

		// Add Safety Margin
		constexpr float SafetyMargin = 0.02f;
		MaxPenetrationDepth = max(0.0f, MaxPenetrationDepth - SafetyMargin);

		// Set Normal & Penetration Depth
		PMeshCollider->SetCollisionNormal(LeftAverageNormal);
		PMeshCollider->SetPenetrationDepth(MaxPenetrationDepth);
		P3DCollider->SetHitNormal(RightAverageNormal);
		P3DCollider->SetPenetrationDepth(MaxPenetrationDepth);

		return true;
	}

	return false;
}

/**
 * @brief 메시 충돌체의 각 정점 중 하나라도 3D 충돌체와 충돌했는지 판단하는 함수
 * Narrow 함수는 순서가 뒤집힌 함수도 필요하다
 * @param P3DCollider 3D Collider
 * @param PMeshCollider Mesh Collider
 * @return 충돌 여부
 */
bool FCollisionManager::IsCollision(CCollider3D* P3DCollider, CMeshCollider* PMeshCollider)
{
	// 1. Variable Setting
	Ptr<CMesh> Mesh = PMeshCollider->GetMesh();

	// Early Return
	if (!Mesh.Get())
	{
		return false;
	}

	UINT VtxCount = Mesh->GetVertexCount();

	// Early Return
	if (!VtxCount)
	{
		return false;
	}

	// 2. Triangle Counting
	INT32 TriCount = 0;
	for (UINT i = 0; i < Mesh->GetSubsetCount(); ++i)
	{
		TriCount += Mesh->GetIndexInfo()[i].IdxCount / 3;
	}

	// Early Return
	if (!TriCount)
	{
		return false;
	}

	// 3. Prepare Buffer
	Matrix WorldMatrix = PMeshCollider->GetOwner()->Transform()->GetWorldMat();

	vector<SimpleVtx> VtxVector(VtxCount);
	Vtx* VtxArr = static_cast<Vtx*>(Mesh->GetVtxSysMem());
	for (UINT i = 0; i < VtxCount; ++i)
	{
		VtxVector[i].pos = XMVector3TransformCoord(VtxArr[i].vPos, WorldMatrix);
	}

	vector<SimpleIdx> IdxVector(TriCount);
	UINT TotalIdx = 0;
	for (UINT i = 0; i < Mesh->GetSubsetCount(); ++i)
	{
		UINT* IdxArr = static_cast<UINT*>(Mesh->GetIndexInfo()[i].IdxSysMem);
		for (size_t j = 0; j < Mesh->GetIndexInfo()[i].IdxCount / 3; ++j)
		{
			IdxVector[TotalIdx] = {IdxArr[j * 3], IdxArr[j * 3 + 1], IdxArr[j * 3 + 2]};
			++TotalIdx;
		}
	}

	// Prepare 3D Collider Buffer
	vector<SimpleVtx> AABBVtxVector(8);
	vector<SimpleIdx> AABBIdxVector(12);

	Vec3 CubeArr[8] = {
		Vec3(-0.5f, 0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f),
		Vec3(0.5f, -0.5f, 0.5f), Vec3(-0.5f, -0.5f, 0.5f),
		Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f),
		Vec3(0.5f, -0.5f, -0.5f), Vec3(-0.5f, -0.5f, -0.5f)
	};

	// 3D Collider Vertex
	for (int i = 0; i < 8; ++i)
	{
		AABBVtxVector[i].pos = XMVector3TransformCoord(CubeArr[i], P3DCollider->GetColliderWorldMat());
	}

	// 3D Collider Triangle Index
	uint32_t Triangles[12][3] = {
		{0, 2, 1}, {0, 3, 2}, {1, 5, 6}, {1, 6, 2},
		{4, 5, 6}, {4, 6, 7}, {0, 4, 7}, {0, 7, 3},
		{4, 5, 1}, {4, 1, 0}, {3, 2, 6}, {3, 6, 7}
	};

	for (int i = 0; i < 12; ++i)
	{
		AABBIdxVector[i] = {Triangles[i][0], Triangles[i][1], Triangles[i][2]};
	}

	// 4. Create StructuredBuffer
	CStructuredBuffer MeshVtxBuffer, MeshIdxBuffer, AABBVtxBuffer, AABBIdxBuffer;
	MeshVtxBuffer.Create(sizeof(SimpleVtx), VtxCount, SB_TYPE::SRV_ONLY, true, VtxVector.data());
	MeshIdxBuffer.Create(sizeof(SimpleIdx), TriCount, SB_TYPE::SRV_ONLY, true, IdxVector.data());
	AABBVtxBuffer.Create(sizeof(SimpleVtx), 8, SB_TYPE::SRV_ONLY, true, AABBVtxVector.data());
	AABBIdxBuffer.Create(sizeof(SimpleIdx), 12, SB_TYPE::SRV_ONLY, true, AABBIdxVector.data());

	CStructuredBuffer CountBuffer, OutputBuffer;

	UINT MaxCollision = 4096;
	vector<UINT> CountArr(1, 0);
	vector<CollisionResult> CollisionArr(MaxCollision);

	CountBuffer.Create(sizeof(UINT), 1, SB_TYPE::SRV_UAV, true, CountArr.data());
	OutputBuffer.Create(sizeof(CollisionResult), MaxCollision, SB_TYPE::SRV_UAV, true, CollisionArr.data());

	// 5. ComputeShader Execute
	MMeshCollisionCS.SetLeftVertices(&MeshVtxBuffer);
	MMeshCollisionCS.SetLeftIndices(&MeshIdxBuffer);
	MMeshCollisionCS.SetRightVertices(&AABBVtxBuffer);
	MMeshCollisionCS.SetRightIndices(&AABBIdxBuffer);
	MMeshCollisionCS.SetCount(&CountBuffer);
	MMeshCollisionCS.SetResults(&OutputBuffer);
	MMeshCollisionCS.SetTriCounts(TriCount, 12);
	MMeshCollisionCS.Execute();

	// 6. Result Check
	CountBuffer.GetData(CountArr.data());
	OutputBuffer.GetData(CollisionArr.data());

	int CollisionCount = CountArr[0];

	if (CollisionCount)
	{
		// Calculate Average Normal with Penetration Depth
		Vec3 LeftAverageNormal = {0, 0, 0};
		Vec3 RightAverageNormal = {0, 0, 0};
		float MaxPenetrationDepth;

		for (int i = 0; i < CollisionCount; ++i)
		{
			Vec3 LeftNormal = CollisionArr[i].LeftNormal;
			Vec3 RightNormal = CollisionArr[i].RightNormal;
			float CurrentPenetration = CollisionArr[i].PenetrationDepth;

			LeftAverageNormal += LeftNormal;
			RightAverageNormal += RightNormal;

			// Update Max Depth
			MaxPenetrationDepth = max(MaxPenetrationDepth, min(CurrentPenetration, 1.0f));
		}

		// Calculate Collision Normal
		LeftAverageNormal = LeftAverageNormal / static_cast<float>(CollisionCount);
		RightAverageNormal = RightAverageNormal / static_cast<float>(CollisionCount);
		LeftAverageNormal.Normalize();
		RightAverageNormal.Normalize();

		// Add Safety Margin
		constexpr float SafetyMargin = 0.02f;
		MaxPenetrationDepth = max(0.0f, MaxPenetrationDepth - SafetyMargin);

		// Set Normal & Penetration Depth
		PMeshCollider->SetCollisionNormal(LeftAverageNormal);
		PMeshCollider->SetPenetrationDepth(MaxPenetrationDepth);
		P3DCollider->SetHitNormal(RightAverageNormal);
		P3DCollider->SetPenetrationDepth(MaxPenetrationDepth);

		return true;
	}

	return false;
}
