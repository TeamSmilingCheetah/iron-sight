#include "pch.h"
#include "Inspector.h"
#include "ComponentUI.h"

#include <Engine/CLevelMgr.h>
#include <Engine/assets.h>

#include "AssetUI.h"
#include "ScriptUI.h"

Inspector::Inspector()
    : EditorUI("Inspector")
      , m_arrComUI{}
{
    CreateComponentUI();

    CreateAssetUI();

    SetTargetObject(nullptr);
}

Inspector::~Inspector()
{
}

void Inspector::Render_Update()
{
    if (nullptr != m_TargetObject)
    {
        if (!IsValid(m_TargetObject))
        {
            SetTargetObject(nullptr);
        }
    }
}

void Inspector::SetTargetObject(CGameObject* _Target)
{
    // 자식 UI 들에게 TargetObject 를 찾아서 알려준다.
    m_TargetObject = _Target;

    for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
    {
        if (nullptr == m_arrComUI[i])
            continue;

        m_arrComUI[i]->SetTargetObject(m_TargetObject);
    }

    // 오브젝트가 소유한 Script 에 대응하는 ScriptUI 를 생성해서 매칭시켜준다.
    if (nullptr == m_TargetObject)
    {
        for (size_t i = 0; i < m_vecScriptUI.size(); ++i)
        {
            m_vecScriptUI[i]->SetScript(nullptr);
        }
    }
    else
    {
        const vector<CScript*>& vecScripts = m_TargetObject->GetScripts();

        for (size_t i = 0; i < vecScripts.size(); ++i)
        {
            if (m_vecScriptUI.size() <= i)
            {
                m_vecScriptUI.push_back(new ScriptUI);
                AddChildUI(m_vecScriptUI.back());
            }

            m_vecScriptUI[i]->SetScript(vecScripts[i]);
        }

        for (size_t i = vecScripts.size(); i < m_vecScriptUI.size(); ++i)
        {
            m_vecScriptUI[i]->SetScript(nullptr);
        }
    }

    // AssetUI 는 비활성화
    if (nullptr != _Target)
        SetTargetAsset(nullptr);
}

void Inspector::SetTargetAsset(Ptr<CAsset> _Asset)
{
    m_TargetAsset = _Asset;

    for (UINT i = 0; i < static_cast<UINT>(ASSET_TYPE::END); ++i)
    {
        if (nullptr == m_TargetAsset || i != m_TargetAsset->GetAssetType())
            m_arrAssetUI[i]->SetActive(false);
        else
        {
            m_arrAssetUI[i]->SetAsset(m_TargetAsset);
            m_arrAssetUI[i]->SetActive(true);
        }
    }

    if (nullptr != m_TargetAsset)
    {
        for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
        {
            if (nullptr == m_arrComUI[i])
                continue;

            m_arrComUI[i]->SetActive(false);
        }
    }
}

#include "TransformUI.h"
#include "Collider2DUI.h"
#include "CameraUI.h"
#include "Light2DUI.h"
#include "MeshRenderUI.h"

void Inspector::CreateComponentUI()
{
    // InspectorUI 의 자식UI 추가
    m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::TRANSFORM)] = static_cast<ComponentUI*>(AddChildUI(
        new TransformUI));
    m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::TRANSFORM)]->SetChildSize(ImVec2(0.f, 150.f));

    m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::COLLIDER2D)] = static_cast<ComponentUI*>(
        AddChildUI(new Collider2DUI));
    m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::COLLIDER2D)]->SetChildSize(ImVec2(0.f, 120.f));

    m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::CAMERA)] = static_cast<ComponentUI*>(AddChildUI(
        new CameraUI));
    m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::CAMERA)]->SetChildSize(ImVec2(0.f, 200.f));

    m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::LIGHT2D)] = static_cast<ComponentUI*>(AddChildUI(
        new Light2DUI));
    m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::LIGHT2D)]->SetChildSize(ImVec2(0.f, 150.f));

    m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::MESHRENDER)] = static_cast<ComponentUI*>(
        AddChildUI(new MeshRenderUI));
    m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::MESHRENDER)]->SetChildSize(ImVec2(0.f, 150.f));
}

#include "MeshUI.h"
#include "MeshDataUI.h"
#include "TextureUI.h"
#include "SoundUI.h"
#include "PrefabUI.h"
#include "FlipbookUI.h"
#include "SpriteUI.h"
#include "MaterialUI.h"
#include "GraphicShaderUI.h"
#include "ComputeShaderUI.h"

void Inspector::CreateAssetUI()
{
    m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::MESH)] = new MeshUI;
    m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::MESH_DATA)] = new MeshDataUI;
    m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::TEXTURE)] = new TextureUI;
    m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::SOUND)] = new SoundUI;
    m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::PREFAB)] = new PrefabUI;
    m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::FLIPBOOK)] = new FlipbookUI;
    m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::SPRITE)] = new SpriteUI;
    m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::MATERIAL)] = new MaterialUI;
    m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::GRAPHIC_SHADER)] = new GraphicShaderUI;
    m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::COMPUTE_SHADER)] = new ComputeShaderUI;

    for (UINT i = 0; i < static_cast<UINT>(ASSET_TYPE::END); ++i)
    {
        AddChildUI(m_arrAssetUI[i]);
    }
}
