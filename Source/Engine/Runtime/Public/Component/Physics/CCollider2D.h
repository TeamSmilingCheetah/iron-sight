#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

class CCollider2D :
    public CComponent
{
private:
    Vec2 m_Offset;
    Vec2 m_Scale;
    Vec2 m_FinalPos;
    Matrix m_matColliderWorld; // 크기, 회전, 이동
    bool m_IndependentScale;

    int m_OverlapCount;

public:
    void SetOffset(Vec2 _Offset) { m_Offset = _Offset; }
    void SetScale(Vec2 _Scale) { m_Scale = _Scale; }

    Vec2 GetOffset() { return m_Offset; }
    Vec2 GetScale() { return m_Scale; }

    const Matrix& GetColliderWorldMat() { return m_matColliderWorld; }

    void SetIndependentScale(bool _Scale) { m_IndependentScale = _Scale; }
    bool IsIndependentScale() { return m_IndependentScale; }

    void FinalTick() override;

    void BeginOverlap(CCollider2D* _Other);
    void Overlap(CCollider2D* _Other);
    void EndOverlap(CCollider2D* _Other);

    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _File) override;

public:
    CLONE(CCollider2D);
    CCollider2D();
    CCollider2D(const CCollider2D& _Origin);
    ~CCollider2D() override;
};
