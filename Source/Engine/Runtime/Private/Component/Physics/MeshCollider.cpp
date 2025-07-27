#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/MeshCollider.h"

FMeshCollider::FMeshCollider()
	: IColliderBase(COMPONENT_TYPE::MESH_COLLIDER)
	  , MMeshPtr(nullptr)
{
}

FMeshCollider::~FMeshCollider() = default;

FMeshCollider::FMeshCollider(const FMeshCollider& POrigin)
	: IColliderBase(COMPONENT_TYPE::MESH_COLLIDER)
	  , MMeshPtr(POrigin.GetMesh())
{
}

void FMeshCollider::Init()
{
	MMeshPtr = GetOwner()->MeshRender()->GetMesh();
}

/**
 * @brief Mesh Collider Final Tick
 *
 * Mesh Collider는 Mesh와 매칭되는 형태로 Final Tick에서는 디버그 렌더링만 처리
 */
void FMeshCollider::FinalTick()
{
	// 메시가 없거나 유효하지 않은 경우 즉시 반환
	if (!MMeshPtr.Get())
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
}

/** Save & Load **/

void FMeshCollider::SaveComponent(FILE* InFile)
{
	if (MMeshPtr->GetKey().empty())
	{
		assert("일단 mesh가 없으면 save 안하는 걸로 정책 설정");
	}
	wstring MeshKey = MMeshPtr->GetKey().empty() ? L"" : MMeshPtr->GetKey();
	SaveWString(MeshKey, InFile);
}

void FMeshCollider::LoadComponent(FILE* InFile)
{
	wstring MeshKey;
	LoadWString(MeshKey, InFile);

	MMeshPtr = CAssetMgr::GetInst()->Load<CMesh>(MeshKey);
}

const AABB FMeshCollider::GetAABB() const
{
	return GetOwner()->GetAABB();
}
