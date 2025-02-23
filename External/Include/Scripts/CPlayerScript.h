#pragma once
#include <Engine/CScript.h>

class CPlayerScript :
    public CScript
{
private:
    float           m_PlayerSpeed;
    float           m_PaperBurnIntence;
    Ptr<CTexture>   m_TargetTex;
    Ptr<CPrefab>    m_Prefab;



public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    CLONE(CPlayerScript);
    CPlayerScript();
    ~CPlayerScript();
};

