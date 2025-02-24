#include "pch.h"
#include "CCollider2D.h"

#include "CTransform.h"
#include "CScript.h"

CCollider2D::CCollider2D()
    : CComponent(COMPONENT_TYPE::COLLIDER2D)
      , m_IndependentScale(false)
      , m_OverlapCount(0)
{
}

CCollider2D::CCollider2D(const CCollider2D& _Origin)
    : CComponent(_Origin)
      , m_Offset(_Origin.m_Offset)
      , m_Scale(_Origin.m_Scale)
      , m_FinalPos(_Origin.m_FinalPos)
      , m_IndependentScale(_Origin.m_IndependentScale)
      , m_OverlapCount(0)
{
}

CCollider2D::~CCollider2D()
{
}

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

void CCollider2D::BeginOverlap(CCollider2D* _Other)
{
    ++m_OverlapCount;

    const vector<CScript*>& vecScript = GetOwner()->GetScripts();
    for (size_t i = 0; i < vecScript.size(); ++i)
    {
        vecScript[i]->BeginOverlap(this, _Other->GetOwner(), _Other);
    }
}

void CCollider2D::Overlap(CCollider2D* _Other)
{
    const vector<CScript*>& vecScript = GetOwner()->GetScripts();
    for (size_t i = 0; i < vecScript.size(); ++i)
    {
        vecScript[i]->Overlap(this, _Other->GetOwner(), _Other);
    }
}

void CCollider2D::EndOverlap(CCollider2D* _Other)
{
    --m_OverlapCount;

    const vector<CScript*>& vecScript = GetOwner()->GetScripts();
    for (size_t i = 0; i < vecScript.size(); ++i)
    {
        vecScript[i]->EndOverlap(this, _Other->GetOwner(), _Other);
    }
}

void CCollider2D::SaveComponent(FILE* _File)
{
    fwrite(&m_Offset, sizeof(Vec2), 1, _File);
    fwrite(&m_Scale, sizeof(Vec2), 1, _File);
    fwrite(&m_FinalPos, sizeof(Vec2), 1, _File);
    fwrite(&m_IndependentScale, sizeof(bool), 1, _File);
}

void CCollider2D::LoadComponent(FILE* _File)
{
    fread(&m_Offset, sizeof(Vec2), 1, _File);
    fread(&m_Scale, sizeof(Vec2), 1, _File);
    fread(&m_FinalPos, sizeof(Vec2), 1, _File);
    fread(&m_IndependentScale, sizeof(bool), 1, _File);
}
