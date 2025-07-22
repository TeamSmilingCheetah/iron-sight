#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider2D.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

// Constant Stucture
constexpr static Vec3 RectArr[4] =
{
	Vec3(-0.5f, 0.5f, 0.f), Vec3(0.5f, 0.5f, 0.f),
	Vec3(0.5f, -0.5f, 0.f), Vec3(-0.5f, -0.5f, 0.f)
};

CCollider2D::CCollider2D()
	: CComponent(COMPONENT_TYPE::COLLIDER2D)
	  , m_IndependentScale(false)
	  , m_OverlapCount(0)
{
}

CCollider2D::CCollider2D(const CCollider2D& POrigin)
	: CComponent(POrigin)
	  , m_Offset(POrigin.m_Offset)
	  , m_Scale(POrigin.m_Scale)
	  , m_FinalPos(POrigin.m_FinalPos)
	  , m_IndependentScale(POrigin.m_IndependentScale)
	  , m_OverlapCount(0)
{
}

CCollider2D::~CCollider2D() = default;

void CCollider2D::FinalTick()
{
	// 크기 * 회전 * 이동
	Matrix matScale = XMMatrixScaling(m_Scale.x, m_Scale.y, 1.f);
	Matrix matTrans = XMMatrixTranslation(m_Offset.x, m_Offset.y, 0.f);

	if (m_IndependentScale)
	{
		Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
		Matrix matScaleInv = XMMatrixInverse(
			nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));
		m_matColliderWorld = matScale * matTrans * matScaleInv * GetOwner()->Transform()->
		                                                                     GetWorldMat();
	}
	else
	{
		m_matColliderWorld = matScale * matTrans * GetOwner()->Transform()->GetWorldMat();
	}

	// DebugRender 요청, 충돌체를 화면에 표시하도록 함
	if (m_OverlapCount)
	{
		DrawDebugRect(Vec4(1.f, 0.f, 0.f, 1.f), m_matColliderWorld, false, 0.f);
	}
	else
	{
		DrawDebugRect(Vec4(0.f, 1.f, 0.f, 1.f), m_matColliderWorld, false, 0.f);
	}
}

void CCollider2D::SaveComponent(FILE* PFile)
{
	(void)fwrite(&m_Offset, sizeof(Vec2), 1, PFile);
	(void)fwrite(&m_Scale, sizeof(Vec2), 1, PFile);
	(void)fwrite(&m_FinalPos, sizeof(Vec2), 1, PFile);
	(void)fwrite(&m_IndependentScale, sizeof(bool), 1, PFile);
}

void CCollider2D::LoadComponent(FILE* PFile)
{
	(void)fread(&m_Offset, sizeof(Vec2), 1, PFile);
	(void)fread(&m_Scale, sizeof(Vec2), 1, PFile);
	(void)fread(&m_FinalPos, sizeof(Vec2), 1, PFile);
	(void)fread(&m_IndependentScale, sizeof(bool), 1, PFile);
}

/**
 * @brief 2D Collider 기준 AABB를 산출하는 함수
 *
 * @param PMin AABB Min
 * @param PMax AABB Max
 */
void CCollider2D::GetAABB(Vec2& PMin, Vec2& PMax) const
{
	// Get 2D Collider Vertexs
	Matrix LeftMatrix = GetColliderWorldMatrix();
	Vec3 Vtxs[4];

	for (int i = 0; i < 4; ++i)
	{
		Vtxs[i] = XMVector3TransformCoord(RectArr[i], LeftMatrix);
	}

	// Get 2D AABB
	PMin = Vtxs->XY();
	PMax = Vtxs->XY();

	for (int i = 1; i < 4; ++i)
	{
		PMin.x = min(PMin.x, Vtxs[i].x);
		PMin.y = min(PMin.y, Vtxs[i].y);
		PMax.x = max(PMax.x, Vtxs[i].x);
		PMax.y = max(PMax.y, Vtxs[i].y);
	}
}
