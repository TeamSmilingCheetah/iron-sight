#pragma once
#include "Engine/System/Public/Asset/Base/CAsset.h"

class CGameObject;

class CPrefab :
    public CAsset
{
public:
    static GAMEOBJECT_SAVE g_ObjectSave;
    static GAMEOBJECT_LOAD g_ObjectLoad;

private:
    CGameObject* m_ProtoObj;

public:
    void SetProtoObject(CGameObject* _Object);
	CGameObject* GetProtoObject() { return m_ProtoObj; }
    CGameObject* Instantiate();

    int Load(const wstring& _FilePath) override;
    int Save(const wstring& _FilePath) override;

    CLONE(CPrefab);
    CPrefab();
    CPrefab(const CPrefab& _Origin);
    ~CPrefab() override;
};
