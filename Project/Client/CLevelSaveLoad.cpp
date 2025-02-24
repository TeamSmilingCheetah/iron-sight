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

    // Level 이름 저장
    _Level->SaveToLevel(pFile);

    // Level 이 보유하고있는 32개의 레이어를 저장시킨다.
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        CLayer* pLayer = _Level->GetLayer(i);

        // 레이어 이름 저장
        pLayer->SaveToLevel(pFile);

        // 레이어가 소유한 오브젝트 정보 저장
        const vector<CGameObject*>& vecObjects = pLayer->GetParentObjects();

        // 오브젝트 총 개수
        size_t ObjectCount = vecObjects.size();
        fwrite(&ObjectCount, sizeof(size_t), 1, pFile);

        // 각 오브젝트의 정보
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
    // 오브젝트 이름
    _Object->SaveToLevel(_File);

    // 오브젝트 컴포넌트
    for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
    {
        CComponent* pComponent = _Object->GetComponent(static_cast<COMPONENT_TYPE>(i));
        if (nullptr == pComponent)
            continue;

        // 컴포넌트 타입 저장
        fwrite(&i, sizeof(UINT), 1, _File);

        // 컴포넌트 데이터 저장
        _Object->GetComponent(static_cast<COMPONENT_TYPE>(i))->SaveToLevel(_File);
    }
    UINT End = static_cast<UINT>(COMPONENT_TYPE::END);
    fwrite(&End, sizeof(UINT), 1, _File);


    // 오브젝트 스크립트
    const vector<CScript*>& vecScripts = _Object->GetScripts();
    size_t ScriptCount = vecScripts.size();
    fwrite(&ScriptCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < vecScripts.size(); ++i)
    {
        // Script 클래스 이름 저장
        wstring ScriptName = CScriptMgr::GetScriptName(vecScripts[i]);
        SaveWString(ScriptName, _File);

        // Script 가 저장해야할 데이터 저장
        vecScripts[i]->SaveToLevel(_File);
    }

    // 자식 오브젝트
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

    // Level 이름 불러오기
    pNewLevel->LoadFromLevel(pFile);

    // Level 이 보유하고있는 32개의 레이어를 불러온다.
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        CLayer* pLayer = pNewLevel->GetLayer(i);

        // 레이어 이름 불러오기
        pLayer->LoadFromLevel(pFile);

        // 레이어가 소유한 오브젝트 불러오기
        const vector<CGameObject*>& vecObjects = pLayer->GetParentObjects();

        // 오브젝트 총 개수
        size_t ObjectCount = 0;
        fread(&ObjectCount, sizeof(size_t), 1, pFile);

        // 각 오브젝트의 정보
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

    // 오브젝트 이름
    pObject->LoadFromLevel(_File);

    // 오브젝트 컴포넌트
    UINT ComponentType = 0;
    while (true)
    {
        // 컴포넌트 타입 로딩
        fread(&ComponentType, sizeof(UINT), 1, _File);

        if (ComponentType == static_cast<UINT>(COMPONENT_TYPE::END))
            break;

        CComponent* pComponent = CreateComponent(static_cast<COMPONENT_TYPE>(ComponentType));

        if (ComponentType == static_cast<UINT>(COMPONENT_TYPE::TRANSFORM))
            pComponent = pObject->Transform();
        else
            pObject->AddComponent(pComponent);

        // 컴포넌트 데이터 저장
        pComponent->LoadFromLevel(_File);
    }

    // 오브젝트 스크립트	
    size_t ScriptCount = 0;
    fread(&ScriptCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < ScriptCount; ++i)
    {
        // Script 클래스 이름을 읽는다.
        wstring ScriptName;
        LoadWString(ScriptName, _File);

        // Script 이름으로 해당 스크립트 객체를 생성 후 GameObject 에 넣어준다.
        CScript* pScript = CScriptMgr::GetScript(ScriptName);
        pObject->AddComponent(pScript);

        // Script 가 저장한 데이터를 다시 복구시킨다.
        pScript->LoadFromLevel(_File);
    }

    // 자식 오브젝트
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
