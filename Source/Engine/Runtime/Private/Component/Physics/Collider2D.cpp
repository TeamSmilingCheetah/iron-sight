#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/Collider2D.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

// Constant Stucture
constexpr static Vec3 RectArr[4] =
{
	Vec3(-0.5f, 0.5f, 0.f), Vec3(0.5f, 0.5f, 0.f),
	Vec3(0.5f, -0.5f, 0.f), Vec3(-0.5f, -0.5f, 0.f)
};

FCollider2D::FCollider2D()
	: IColliderBase(COMPONENT_TYPE::COLLIDER2D)
	  , bIndependentScale(false)
{
}

FCollider2D::FCollider2D(const FCollider2D& POrigin)
	: IColliderBase(POrigin)
	  , Offset(POrigin.Offset)
	  , Scale(POrigin.Scale)
	  , FinalPos(POrigin.FinalPos)
	  , bIndependentScale(POrigin.bIndependentScale)
{
}

FCollider2D::~FCollider2D() = default;

void FCollider2D::FinalTick()
{
	// 크기 * 회전 * 이동
	Matrix matScale = XMMatrixScaling(Scale.x, Scale.y, 1.f);
	Matrix matTrans = XMMatrixTranslation(Offset.x, Offset.y, 0.f);

	if (bIndependentScale)
	{
		Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
		Matrix matScaleInv = XMMatrixInverse(
			nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));
		MatrixColliderWorld = matScale * matTrans * matScaleInv * GetOwner()->Transform()->
		                                                                     GetWorldMat();
	}
	else
	{
		MatrixColliderWorld = matScale * matTrans * GetOwner()->Transform()->GetWorldMat();
	}

	// DebugRender 요청, 충돌체를 화면에 표시하도록 함
	if (IsOverlapped())
	{
		DrawDebugRect(Vec4(1.f, 0.f, 0.f, 1.f), MatrixColliderWorld, false, 0.f);
	}
	else
	{
		DrawDebugRect(Vec4(0.f, 1.f, 0.f, 1.f), MatrixColliderWorld, false, 0.f);
	}
}

void FCollider2D::SaveComponent(FILE* InFile)
{
	(void)fwrite(&Offset, sizeof(Vec2), 1, InFile);
	(void)fwrite(&Scale, sizeof(Vec2), 1, InFile);
	(void)fwrite(&FinalPos, sizeof(Vec2), 1, InFile);
	(void)fwrite(&bIndependentScale, sizeof(bool), 1, InFile);
}

void FCollider2D::LoadComponent(FILE* InFile)
{
	(void)fread(&Offset, sizeof(Vec2), 1, InFile);
	(void)fread(&Scale, sizeof(Vec2), 1, InFile);
	(void)fread(&FinalPos, sizeof(Vec2), 1, InFile);
	(void)fread(&bIndependentScale, sizeof(bool), 1, InFile);
}

/**
 * @brief 2D Collider 기준 AABB를 산출하는 함수
 *
 * @return 2차원 충돌체의 AABB 값
 */
const AABB FCollider2D::GetAABB() const
{
	// Get 2D Collider Vertexs
	Matrix LeftMatrix = GetColliderWorldMatrix();
	Vec3 Vtxs[4];

	for (int i = 0; i < 4; ++i)
	{
		Vtxs[i] = XMVector3TransformCoord(RectArr[i], LeftMatrix);
	}

	// Get 2D AABB
	Vec3 PMin = Vtxs[0];
	Vec3 PMax = Vtxs[0];

	for (int i = 1; i < 4; ++i)
	{
		PMin.x = min(PMin.x, Vtxs[i].x);
		PMin.y = min(PMin.y, Vtxs[i].y);
		PMax.x = max(PMax.x, Vtxs[i].x);
		PMax.y = max(PMax.y, Vtxs[i].y);
	}

	return AABB(PMin, PMax);
}
