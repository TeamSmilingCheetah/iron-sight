#pragma once
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Base/CComponent.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"

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
	COLLIDER_STATE m_State = ACTIVE;

public:
    void SetOffset(Vec2 POffset) { m_Offset = POffset; }
    void SetScale(Vec2 PScale) { m_Scale = PScale; }

    Vec2 GetOffset() const { return m_Offset; }
    Vec2 GetScale() const { return m_Scale; }
	void GetAABB(Vec2& PMin, Vec2& PMax) const;
	COLLIDER_STATE GetState() const { return m_State; }

    const Matrix& GetColliderWorldMatrix() const { return m_matColliderWorld; }

    void SetIndependentScale(bool PScale) { m_IndependentScale = PScale; }
    bool IsIndependentScale() const { return m_IndependentScale; }

    void FinalTick() override;

	// Templated Overlap Function
	template <typename T>
	void BeginOverlap(T* POther);
	template <typename T>
	void Overlap(T* POther);
	template <typename T>
	void EndOverlap(T* POther);

    void SaveComponent(FILE* PFile) override;
    void LoadComponent(FILE* PFile) override;

public:
    CLONE(CCollider2D);
    CCollider2D();
    CCollider2D(const CCollider2D& POrigin);
    ~CCollider2D() override;
};

template <typename T>
void CCollider2D::BeginOverlap(T* POther)
{
	++m_OverlapCount;

	const vector<CScript*>& ScriptVector = GetOwner()->GetScripts();
	for (auto* Script : ScriptVector)
	{
		Script->BeginOverlap(this, POther->GetOwner(), POther);
	}
}

template <typename T>
void CCollider2D::Overlap(T* POther)
{
	const vector<CScript*>& ScriptVector = GetOwner()->GetScripts();
	for (auto* Script : ScriptVector)
	{
		Script->Overlap(this, POther->GetOwner(), POther);
	}
}

template <typename T>
void CCollider2D::EndOverlap(T* POther)
{
	--m_OverlapCount;

	const vector<CScript*>& ScriptVector = GetOwner()->GetScripts();
	for (auto* Script : ScriptVector)
	{
		Script->EndOverlap(this, POther->GetOwner(), POther);
	}
}
