#include "pch.h"
#include "CLevelSaveLoad.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/components.h>
#include <Engine/CScript.h>

#include <Scripts/CScriptMgr.h>

int CLevelSaveLoad::SaveLevel(const wstring& _FilePath, CLevel* _Level)
{
    FILE* pFile = nullptr;

    _wfopen_s(&pFile, _FilePath.c_str(), L"wb");
    assert(pFile);

    // Level �̸� ����
    _Level->SaveToLevel(pFile);

    // Level �� �����ϰ��ִ� 32���� ���̾ �����Ų��.
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        CLayer* pLayer = _Level->GetLayer(i);

        // ���̾� �̸� ����
        pLayer->SaveToLevel(pFile);

        // ���̾ ������ ������Ʈ ���� ����
        const vector<CGameObject*>& vecObjects = pLayer->GetParentObjects();

        // ������Ʈ �� ����
        size_t ObjectCount = vecObjects.size();
        fwrite(&ObjectCount, sizeof(size_t), 1, pFile);

        // �� ������Ʈ�� ����
        for (size_t j = 0; j < vecObjects.size(); ++j)
        {
            SaveGameObject(vecObjects[j], pFile);
        }
    }

    fclose(pFile);

    return S_OK;
}

int CLevelSaveLoad::SaveGameObject(CGameObject* _Object, FILE* _File)
{
    // ������Ʈ �̸�
    _Object->SaveToLevel(_File);

    // ������Ʈ ������Ʈ
    for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
    {
        CComponent* pComponent = _Object->GetComponent(static_cast<COMPONENT_TYPE>(i));
        if (nullptr == pComponent)
            continue;

        // ������Ʈ Ÿ�� ����
        fwrite(&i, sizeof(UINT), 1, _File);

        // ������Ʈ ������ ����
        _Object->GetComponent(static_cast<COMPONENT_TYPE>(i))->SaveToLevel(_File);
    }
    UINT End = static_cast<UINT>(COMPONENT_TYPE::END);
    fwrite(&End, sizeof(UINT), 1, _File);


    // ������Ʈ ��ũ��Ʈ
    const vector<CScript*>& vecScripts = _Object->GetScripts();
    size_t ScriptCount = vecScripts.size();
    fwrite(&ScriptCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < vecScripts.size(); ++i)
    {
        // Script Ŭ���� �̸� ����
        wstring ScriptName = CScriptMgr::GetScriptName(vecScripts[i]);
        SaveWString(ScriptName, _File);

        // Script �� �����ؾ��� ������ ����
        vecScripts[i]->SaveToLevel(_File);
    }

    // �ڽ� ������Ʈ
    const vector<CGameObject*>& vecChild = _Object->GetChild();
    size_t ChildCount = vecChild.size();
    fwrite(&ChildCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < vecChild.size(); ++i)
    {
        SaveGameObject(vecChild[i], _File);
    }

    return S_OK;
}

CLevel* CLevelSaveLoad::LoadLevel(const wstring& _FilePath)
{
    FILE* pFile = nullptr;

    auto pNewLevel = new CLevel;

    _wfopen_s(&pFile, _FilePath.c_str(), L"rb");
    assert(pFile);

    // Level �̸� �ҷ�����
    pNewLevel->LoadFromLevel(pFile);

    // Level �� �����ϰ��ִ� 32���� ���̾ �ҷ��´�.
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        CLayer* pLayer = pNewLevel->GetLayer(i);

        // ���̾� �̸� �ҷ�����
        pLayer->LoadFromLevel(pFile);

        // ���̾ ������ ������Ʈ �ҷ�����
        const vector<CGameObject*>& vecObjects = pLayer->GetParentObjects();

        // ������Ʈ �� ����
        size_t ObjectCount = 0;
        fread(&ObjectCount, sizeof(size_t), 1, pFile);

        // �� ������Ʈ�� ����
        for (size_t j = 0; j < ObjectCount; ++j)
        {
            CGameObject* pNewObject = LoadGameObject(pFile);
            pLayer->AddObject(pNewObject, false);
        }
    }

    fclose(pFile);

    return pNewLevel;
}

CGameObject* CLevelSaveLoad::LoadGameObject(FILE* _File)
{
    auto pObject = new CGameObject;

    // ������Ʈ �̸�
    pObject->LoadFromLevel(_File);

    // ������Ʈ ������Ʈ
    UINT ComponentType = 0;
    while (true)
    {
        // ������Ʈ Ÿ�� �ε�
        fread(&ComponentType, sizeof(UINT), 1, _File);

        if (ComponentType == static_cast<UINT>(COMPONENT_TYPE::END))
            break;

        CComponent* pComponent = CreateComponent(static_cast<COMPONENT_TYPE>(ComponentType));

        if (ComponentType == static_cast<UINT>(COMPONENT_TYPE::TRANSFORM))
            pComponent = pObject->Transform();
        else
            pObject->AddComponent(pComponent);

        // ������Ʈ ������ ����
        pComponent->LoadFromLevel(_File);
    }

    // ������Ʈ ��ũ��Ʈ	
    size_t ScriptCount = 0;
    fread(&ScriptCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < ScriptCount; ++i)
    {
        // Script Ŭ���� �̸��� �д´�.
        wstring ScriptName;
        LoadWString(ScriptName, _File);

        // Script �̸����� �ش� ��ũ��Ʈ ��ü�� ���� �� GameObject �� �־��ش�.
        CScript* pScript = CScriptMgr::GetScript(ScriptName);
        pObject->AddComponent(pScript);

        // Script �� ������ �����͸� �ٽ� ������Ų��.
        pScript->LoadFromLevel(_File);
    }

    // �ڽ� ������Ʈ
    size_t ChildCount = 0;
    fread(&ChildCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < ChildCount; ++i)
    {
        CGameObject* pChild = LoadGameObject(_File);
        pObject->AddChild(pChild);
    }

    return pObject;
}

CComponent* CLevelSaveLoad::CreateComponent(COMPONENT_TYPE _Type)
{
    switch (_Type)
    {
    case COMPONENT_TYPE::COLLIDER2D:
        return new CCollider2D;
    //case COMPONENT_TYPE::COLLIDER3D:
    case COMPONENT_TYPE::FLIPBOOKPLAYER:
        return new CFlipbookPlayer;
    //case COMPONENT_TYPE::ANIMATOR3D:
    case COMPONENT_TYPE::CAMERA:
        return new CCamera;
    case COMPONENT_TYPE::LIGHT2D:
        return new CLight2D;
    case COMPONENT_TYPE::LIGHT3D:
        return new CLight3D;
    //case COMPONENT_TYPE::STATEMACINE:
    case COMPONENT_TYPE::MESHRENDER:
        return new CMeshRender;
    case COMPONENT_TYPE::TILEMAP:
        return new CTileMap;
    case COMPONENT_TYPE::PARTICLE_SYSTEM:
        return new CParticleSystem;
    //case COMPONENT_TYPE::SKYBOX:
    //case COMPONENT_TYPE::DECAL:
    //case COMPONENT_TYPE::LANDSCALE:
    }

    return nullptr;
}
