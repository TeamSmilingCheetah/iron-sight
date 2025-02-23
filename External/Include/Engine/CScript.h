#pragma once
#include "CComponent.h"

#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CAssetMgr.h"
#include "components.h"


struct tScriptParam
{
    SCRIPT_PARAM    Param;
    string          Desc;
    void*           pData;
};


class CScript :
    public CComponent
{
private:
    const UINT              m_ScriptType;
    vector<tScriptParam>    m_vecScriptParam;

public:
    UINT GetScriptType() { return m_ScriptType; }
    void AddScriptParam(tScriptParam _Param) { m_vecScriptParam.push_back(_Param); }
    const vector<tScriptParam>& GetScriptParam() { return m_vecScriptParam; }
    void Instantiate(Ptr<CPrefab> _Pref, Vec3 _WorldPos, int _Layer);


public:
    virtual void Tick() = 0;
    virtual void FinalTick() final {}

    virtual void BeginOverlap(class CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {};
    virtual void Overlap(class CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {};
    virtual void EndOverlap(class CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {};


public:
    CScript(UINT _ScriptType);
    ~CScript();
};

