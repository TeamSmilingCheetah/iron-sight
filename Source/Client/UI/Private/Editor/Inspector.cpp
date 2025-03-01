#include "pch.h"
#include "UI/Public/Editor/Inspector.h"
#include "Rendering/Public/ComputeShaderUI.h"
#include "Rendering/Public/GraphicShaderUI.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "System/Public/Asset/Base/CAsset.h"
#include "UI/Public/Asset/AssetUI.h"
#include "UI/Public/Asset/FlipbookUI.h"
#include "UI/Public/Asset/MaterialUI.h"
#include "UI/Public/Asset/MeshDataUI.h"
#include "UI/Public/Asset/MeshUI.h"
#include "UI/Public/Asset/PrefabUI.h"
#include "UI/Public/Asset/SoundUI.h"
#include "UI/Public/Asset/SpriteUI.h"
#include "UI/Public/Asset/TextureUI.h"
#include "UI/Public/Component/Animator3DUI.h"
#include "UI/Public/Component/CameraUI.h"
#include "UI/Public/Component/Collider2DUI.h"
#include "UI/Public/Component/ComponentUI.h"
#include "UI/Public/Component/LandScapeUI.h"
#include "UI/Public/Component/Light2DUI.h"
#include "UI/Public/Component/Light3DUI.h"
#include "UI/Public/Component/MeshRenderUI.h"
#include "UI/Public/Component/ScriptUI.h"
#include "UI/Public/Component/SkyBoxUI.h"
#include "UI/Public/Component/TransformUI.h"

class CScript;

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

	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::LIGHT3D)] = static_cast<ComponentUI*>(
		AddChildUI(new Light3DUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::LIGHT3D)]->SetChildSize(ImVec2(0.f, 150.f));

	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::ANIMATOR3D)] = static_cast<ComponentUI*>(
		AddChildUI(new Animator3DUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::ANIMATOR3D)]->SetChildSize(ImVec2(0.f, 150.f));

	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::SKYBOX)] = static_cast<ComponentUI*>(
		AddChildUI(new SkyBoxUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::SKYBOX)]->SetChildSize(ImVec2(0.f, 150.f));

	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::LANDSCAPE)] = static_cast<ComponentUI*>(
		AddChildUI(new LandScapeUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::LANDSCAPE)]->SetChildSize(ImVec2(0.f, 150.f));
}

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
	m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::ANIMATION)] = new ComputeShaderUI;
	m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::SKELETON)] = new ComputeShaderUI;


	for (UINT i = 0; i < static_cast<UINT>(ASSET_TYPE::END); ++i)
	{
		AddChildUI(m_arrAssetUI[i]);
	}
}
