#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/MeshCollider.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Physics/PhysicsHelper.h"
#include "Runtime/Public/Component/Transform/CTransform.h"

FMeshCollider::FMeshCollider()
	: IColliderBase(COMPONENT_TYPE::MESH_COLLIDER)
	  , MeshPtr(nullptr)
	  , bIsUseOriginalMesh(false)
{
}

FMeshCollider::~FMeshCollider() = default;

FMeshCollider::FMeshCollider(const FMeshCollider& InOrigin)
	: IColliderBase(COMPONENT_TYPE::MESH_COLLIDER)
	  , MeshPtr(InOrigin.GetMesh())
	  , bIsUseOriginalMesh(InOrigin.bIsUseOriginalMesh)
{
	GenerateConvexHull();
}

void FMeshCollider::Init()
{
	MeshPtr = GetOwner()->MeshRender()->GetMesh();
	GenerateConvexHull();
}

/**
 * @brief PhysicsHelper를 통해 Convex Hull Mesh를 생성하고, 보유하도록 하는 함수
 */
void FMeshCollider::GenerateConvexHull()
{
	if (!MeshPtr.Get())
	{
		ConvexHullMeshPtr = nullptr;
		LOG_ERROR_F("[Collider][Mesh] {}: Can't Generate Convex Hull Mesh", WStringToString(GetOwner()->GetName()));

		return;
	}

	ConvexHullMeshPtr = FPhysicsHelper::CreateConvexHullFromMesh(MeshPtr);

	if (!ConvexHullMeshPtr.Get())
	{
		LOG_ERROR_F("[Collider][Mesh] {}: Failed To Generate Convex Hull",
		            WStringToString(GetOwner()->GetName()));
	}
}

/**
 * @brief Mesh Collider Final Tick
 *
 * Mesh Collider는 Mesh와 매칭되는 형태로 Final Tick에서는 디버그 렌더링만 처리
 */
void FMeshCollider::FinalTick()
{
	// 메시가 없거나 유효하지 않은 경우 즉시 반환
	if (!MeshPtr.Get())
	{
		LOG_CRITICAL_F("[Collision][MeshCollider] {}: Don't Have Mesh But Enter MeshCollider FinalTick",
		               WStringToString(GetOwner()->GetName()));
		assert(!"Mesh가 없는데 Mesh Collider FinalTick 처리 분기로 들어옴");
		return;
	}

	// 디버그 렌더링을 위한 매트릭스 처리
	const auto& BoundingBox = GetOwner()->GetAABB();
	Vec3 BoxMin = BoundingBox.Min;
	Vec3 BoxMax = BoundingBox.Max;

	Vec3 BoxSize = BoxMax - BoxMin;
	Vec3 BoxCenter = (BoxMin + BoxMax) * 0.5f;
	Matrix MatrixScale = XMMatrixScaling(BoxSize.x, BoxSize.y, BoxSize.z);
	Matrix MatrixTrans = XMMatrixTranslation(BoxCenter.x, BoxCenter.y, BoxCenter.z);

	Matrix DebugMatrix = MatrixScale * MatrixTrans;

	// 색상 판정
	Vec4 CubeColor;
	if (IsOverlapped())
	{
		// 충돌 중인 경우 빨간색으로 표시
		CubeColor = Vec4(1.f, 0.f, 0.f, 1.f);
	}
	else
	{
		// 충돌하지 않는 경우 초록색으로 표시
		CubeColor = Vec4(0.f, 1.f, 0.f, 1.f);
	}

	// 디버그 렌더링
	DrawDebugCube(CubeColor, DebugMatrix, false, 0.f);

	// TODO(KHJ): 드로우콜 최적화 완료되면 정상 렌더링 시도해볼 것
	// 현재는 CPU 연산량이 커서 그런지 프레임 확보가 안됨

	// // 색상 판정
	// Vec4 WireframeColor;
	// if (IsOverlapped())
	// {
	// 	// 충돌 중인 경우 빨간색으로 표시
	// 	WireframeColor = Vec4(1.f, 0.f, 0.f, 1.f);
	// }
	// else
	// {
	// 	// 충돌하지 않는 경우 초록색으로 표시
	// 	WireframeColor = Vec4(0.f, 1.f, 0.f, 1.f);
	// }
	//
	// Vtx* Vertices = static_cast<Vtx*>(ConvexHullMeshPtr->GetVtxSysMem());
	// UINT VertexCount = ConvexHullMeshPtr->GetVertexCount();
	//
	// if (Vertices && VertexCount > 0)
	// {
	// 	Matrix WorldMatrix = GetOwner()->Transform()->GetWorldMat();
	//
	// 	UINT SubsetCount = ConvexHullMeshPtr->GetSubsetCount();
	// 	for (UINT SubsetIdx = 0; SubsetIdx < SubsetCount; ++SubsetIdx)
	// 	{
	// 		auto& IndexInfo = ConvexHullMeshPtr->GetIndexInfo()[SubsetIdx];
	// 		UINT* Indices = static_cast<UINT*>(IndexInfo.IdxSysMem);
	// 		UINT IndexCount = IndexInfo.IdxCount;
	//
	// 		if (Indices && IndexCount > 0)
	// 		{
	// 			// 삼각형 단위로 와이어프레임 렌더링
	// 			for (UINT i = 0; i < IndexCount; i += 3)
	// 			{
	// 				if (i + 2 < IndexCount)
	// 				{
	// 					// 삼각형의 세 정점 인덱스
	// 					UINT Idx0 = Indices[i];
	// 					UINT Idx1 = Indices[i + 1];
	// 					UINT Idx2 = Indices[i + 2];
	//
	// 					// 정점 위치를 월드 좌표로 변환
	// 					Vec3 Vertex0 = Vertices[Idx0].vPos;
	// 					Vec3 Vertex1 = Vertices[Idx1].vPos;
	// 					Vec3 Vertex2 = Vertices[Idx2].vPos;
	//
	// 					// 월드 매트릭스 적용
	// 					Vec4 WorldVertex0 = XMVector3Transform(Vec4(Vertex0, 1.f), WorldMatrix);
	// 					Vec4 WorldVertex1 = XMVector3Transform(Vec4(Vertex1, 1.f), WorldMatrix);
	// 					Vec4 WorldVertex2 = XMVector3Transform(Vec4(Vertex2, 1.f), WorldMatrix);
	//
	// 					// 삼각형의 세 변을 선으로 렌더링
	// 					DrawDebugLine(WireframeColor, WorldVertex0, WorldVertex1, false, 0.f);
	// 					DrawDebugLine(WireframeColor, WorldVertex1, WorldVertex2, false, 0.f);
	// 					DrawDebugLine(WireframeColor, WorldVertex2, WorldVertex0, false, 0.f);
	// 				}
	// 			}
	// 		}
	// 	}
	// }
}

/** Save & Load **/

void FMeshCollider::SaveComponent(FILE* InFile)
{
	if (MeshPtr->GetKey().empty())
	{
		assert("일단 mesh가 없으면 save 안하는 걸로 정책 설정");
	}
	wstring MeshKey = MeshPtr->GetKey().empty() ? L"" : MeshPtr->GetKey();
	SaveWString(MeshKey, InFile);

	bool IsStaticCollider = IsStatic();
	(void)fwrite(&IsStaticCollider, sizeof(bool), 1, InFile);
}

void FMeshCollider::LoadComponent(FILE* InFile)
{
	wstring MeshKey;
	LoadWString(MeshKey, InFile);

	MeshPtr = CAssetMgr::GetInst()->Load<CMesh>(MeshKey);

	bool IsStaticCollider;
	(void)fread(&IsStaticCollider, sizeof(bool), 1, InFile);

	if (!IsStaticCollider)
	{
		SetDynamic();
	}
	else
	{
		SetStatic();
	}
}

const AABB FMeshCollider::GetAABB() const
{
	return GetOwner()->GetAABB();
}
