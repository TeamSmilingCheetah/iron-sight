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
    void SetOffset(Vec2 POffset) { m_Offset = POffset; }
    void SetScale(Vec2 PScale) { m_Scale = PScale; }

    Vec2 GetOffset() const { return m_Offset; }
    Vec2 GetScale() const { return m_Scale; }
	void GetAABB(Vec2& PMin, Vec2& PMax) const;

    const Matrix& GetColliderWorldMatrix() const { return m_matColliderWorld; }

    void SetIndependentScale(bool PScale) { m_IndependentScale = PScale; }
    bool IsIndependentScale() const { return m_IndependentScale; }

    void FinalTick() override;

    void BeginOverlap(CCollider2D* POther);
    void Overlap(CCollider2D* POther);
    void EndOverlap(CCollider2D* POther);

    void SaveComponent(FILE* PFile) override;
    void LoadComponent(FILE* PFile) override;

public:
    CLONE(CCollider2D);
    CCollider2D();
    CCollider2D(const CCollider2D& POrigin);
    ~CCollider2D() override;
};
