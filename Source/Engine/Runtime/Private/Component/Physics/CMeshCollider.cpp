#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/CMeshCollider.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"

CMeshCollider::CMeshCollider()
	: CComponent(COMPONENT_TYPE::MESH_COLLIDER)
	  , MMeshPtr(nullptr)
	  , MOverlapCount(0)
{
}

CMeshCollider::~CMeshCollider() = default;

CMeshCollider::CMeshCollider(const CMeshCollider& POrigin)
	: CComponent(COMPONENT_TYPE::MESH_COLLIDER)
	  , MMeshPtr(POrigin.GetMesh())
	  , MOverlapCount(POrigin.MOverlapCount)
{
}

/**
 * @brief Mesh Collider Final Tick
 *
 * Mesh Collider는 Mesh와 매칭되는 형태로 Final Tick에서는 디버그 렌더링만 처리
 */
void CMeshCollider::FinalTick()
{
	// 메시가 없거나 유효하지 않은 경우 즉시 반환
	if (!MMeshPtr.Get())
	{
#ifdef _DEBUG
		{
			assert("Mesh가 없는데 Mesh Collider FinalTick 처리 분기로 들어옴");
		}
#endif
		return;
	}

	// 디버그 렌더링을 위한 매트릭스 처리
	Vec3 BoxMin;
	Vec3 BoxMax;
	GetOwner()->GetWorldBoundingBox(BoxMin, BoxMax);

	Vec3 BoxSize = BoxMax - BoxMin;
	Vec3 BoxCenter = (BoxMin + BoxMax) * 0.5f;
	Matrix MatrixScale = XMMatrixScaling(BoxSize.x, BoxSize.y, BoxSize.z);
	Matrix MatrixTrans = XMMatrixTranslation(BoxCenter.x, BoxCenter.y, BoxCenter.z);

	Matrix DebugMatrix = MatrixScale * MatrixTrans;

	// 색상 판정
	Vec4 CubeColor;
	if (MOverlapCount)
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

void CMeshCollider::SaveComponent(FILE* PFile)
{
	if (MMeshPtr->GetKey().empty())
	{
		assert("일단 mesh가 없으면 save 안하는 걸로 정책 설정");
	}
	wstring MeshKey = MMeshPtr->GetKey().empty() ? L"" : MMeshPtr->GetKey();
	SaveWString(MeshKey, PFile);
}

void CMeshCollider::LoadComponent(FILE* PFile)
{
	wstring MeshKey;
	LoadWString(MeshKey, PFile);

	MMeshPtr = CAssetMgr::GetInst()->Load<CMesh>(MeshKey);
}

void CMeshCollider::Init()
{
	MMeshPtr = GetOwner()->MeshRender()->GetMesh();
}
