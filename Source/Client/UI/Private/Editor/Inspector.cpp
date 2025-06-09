#include "pch.h"
#include "Client/UI/Public/Editor/Inspector.h"
#include "Client/Rendering/Public/ComputeShaderUI.h"
#include "Client/Rendering/Public/GraphicShaderUI.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/System/Public/Asset/Base/CAsset.h"
#include "Engine/Runtime/Public/Component/Base/components.h"
#include "Client/UI/Public/Asset/AssetUI.h"
#include "Client/UI/Public/Asset/FlipbookUI.h"
#include "Client/UI/Public/Asset/AnimationUI.h"
#include "Client/UI/Public/Asset/MaterialUI.h"
#include "Client/UI/Public/Asset/MeshDataUI.h"
#include "Client/UI/Public/Asset/MeshUI.h"
#include "Client/UI/Public/Asset/PrefabUI.h"
#include "Client/UI/Public/Asset/SoundUI.h"
#include "Client/UI/Public/Asset/SpriteUI.h"
#include "Client/UI/Public/Asset/TextureUI.h"
#include "Client/UI/Public/Component/Animator3DUI.h"
#include "Client/UI/Public/Component/CameraUI.h"
#include "Client/UI/Public/Component/Collider2DUI.h"
#include "Client/UI/Public/Component/Collider3DUI.h"
#include "Client/UI/Public/Component/ColliderRayUI.h"
#include "Client/UI/Public/Component/ComponentUI.h"
#include "Client/UI/Public/Component/LandScapeUI.h"
#include "Client/UI/Public/Component/Light2DUI.h"
#include "Client/UI/Public/Component/Light3DUI.h"
#include "Client/UI/Public/Component/MeshRenderUI.h"
#include "Client/UI/Public/Component/ScriptUI.h"
#include "Client/UI/Public/Component/SkyBoxUI.h"
#include "Client/UI/Public/Component/DecalUI.h"
#include "Client/UI/Public/Component/TransformUI.h"
#include "Client/UI/Public/Component/ParticleUI.h"

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
	// 대상이 오브젝트일 경우 활성
	if (nullptr != m_TargetObject)
	{
		if (!IsValid(m_TargetObject))
		{
			SetTargetObject(nullptr);
		}
		else
		{
			// 오브젝트 활성화 체크박스
			bool active = m_TargetObject->IsActive();
			if (ImGui::Checkbox("##isActive", &active))
			{
				SetObjectActive(m_TargetObject, active);
			}

			// 오브젝트 삭제 버튼
			ImGui::SameLine(30);
			if (ImGui::Button("DeleteObject"))
			{
				DestroyObject(m_TargetObject);
				SetTargetObject(nullptr);
			}

			// Component 추가
			vector<const char*> szProjType;
			vector<string>		strStorage;

			strStorage.reserve(static_cast<UINT>(COMPONENT_TYPE::END) - 1);

			for (UINT i = 1; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
			{
				string ComponentName;
				switch ((COMPONENT_TYPE)i)
				{
				case COMPONENT_TYPE::CAMERA:
					ComponentName = "Camera";
					break;
				case COMPONENT_TYPE::COLLIDER2D:
					ComponentName = "Collider2D";
					break;
				case COMPONENT_TYPE::COLLIDER3D:
					ComponentName = "Collider3D";
					break;
				case COMPONENT_TYPE::COLLIDERRAY:
					ComponentName = "ColliderRay";
					break;
				case COMPONENT_TYPE::ANIMATOR3D:
					ComponentName = "Animator3D";
					break;
				case COMPONENT_TYPE::FLIPBOOKPLAYER:
					ComponentName = "FlipbookPlayer";
					break;
				case COMPONENT_TYPE::LIGHT2D:
					ComponentName = "Light2D";
					break;
				case COMPONENT_TYPE::LIGHT3D:
					ComponentName = "Light3D";
					break;
				case COMPONENT_TYPE::STATEMACINE:
					ComponentName = "StateMachine";
					break;
				case COMPONENT_TYPE::MESHRENDER:
					ComponentName = "MeshRender";
					break;
				case COMPONENT_TYPE::TILEMAP:
					ComponentName = "TileMap";
					break;
				case COMPONENT_TYPE::SKYBOX:
					ComponentName = "SkyBox";
					break;
				case COMPONENT_TYPE::DECAL:
					ComponentName = "Decal";
					break;
				case COMPONENT_TYPE::LANDSCAPE:
					ComponentName = "LandScape";
					break;
				case COMPONENT_TYPE::PARTICLE_SYSTEM:
					ComponentName = "ParticleSystem";
					break;
				case COMPONENT_TYPE::UI:
					ComponentName = "UI";
					break;
				case COMPONENT_TYPE::UIRENDER:
					ComponentName = "UIRender";
					break;
				}
				strStorage.push_back(ComponentName);  // string을 저장하여 유효하게 유지
				szProjType.push_back(strStorage[i - 1].c_str());  // c_str()로 const char* 저장
			}

			ImGui::Combo("##Componentcombo", &m_ComponentListIdx, szProjType.data(), static_cast<int>(szProjType.size()));    // 이름, 선택된 배열 위치, 배열, 개수

			ImGui::SameLine();

			if (ImGui::Button("AddComponent"))
			{
				COMPONENT_TYPE type = (COMPONENT_TYPE)(m_ComponentListIdx + 1);
				CComponent* pCom = m_TargetObject->GetComponent(type);
				if (nullptr == pCom) 
				{
					AddComponent(type);
				}
			}
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

void Inspector::AddComponent(COMPONENT_TYPE _Type)
{
	switch (_Type)
	{
	case COMPONENT_TYPE::CAMERA:
		m_TargetObject->AddComponent(new CCamera);
		break;
	case COMPONENT_TYPE::COLLIDER2D:
		m_TargetObject->AddComponent(new CCollider2D);
		break;
	case COMPONENT_TYPE::FLIPBOOKPLAYER:
		m_TargetObject->AddComponent(new CFlipbookPlayer);
		break;
	case COMPONENT_TYPE::LIGHT2D:
		m_TargetObject->AddComponent(new CLight2D);
		break;
	case COMPONENT_TYPE::MESHRENDER:
		m_TargetObject->AddComponent(new CMeshRender);
		break;
	case COMPONENT_TYPE::TILEMAP:
		m_TargetObject->AddComponent(new CTileMap);
		break;
	case COMPONENT_TYPE::PARTICLE_SYSTEM:
		m_TargetObject->AddComponent(new CParticleSystem);
		break;
	case COMPONENT_TYPE::COLLIDER3D:
		m_TargetObject->AddComponent(new CCollider3D);
		break;
	case COMPONENT_TYPE::COLLIDERRAY:
		m_TargetObject->AddComponent(new CColliderRay);
		break;
	case COMPONENT_TYPE::ANIMATOR3D:
		m_TargetObject->AddComponent(new CAnimator3D);
		break;
	case COMPONENT_TYPE::LIGHT3D:
		m_TargetObject->AddComponent(new CLight3D);
		break;
	case COMPONENT_TYPE::STATEMACINE:
		//m_TargetObject->AddComponent(new CStateMachine);
		break;
	case COMPONENT_TYPE::SKYBOX:
		m_TargetObject->AddComponent(new CSkyBox);
		break;
	case COMPONENT_TYPE::DECAL:
		m_TargetObject->AddComponent(new CDecal);
		break;
	case COMPONENT_TYPE::LANDSCAPE:
		m_TargetObject->AddComponent(new CLandScape);
		break;
	case COMPONENT_TYPE::UI:
		m_TargetObject->AddComponent(new CUI);
		break;
	case COMPONENT_TYPE::UIRENDER:
		m_TargetObject->AddComponent(new CUIRender);
		break;
	}


	SetTargetObject(m_TargetObject);
}

void Inspector::AddScript(SCRIPT_TYPE _Type)
{
	CScript* pScripttype = GameplayManager::GetScript((UINT)_Type);

	const vector<CScript*>& pvecScripts = m_TargetObject->GetScripts();

	// 같은 스크립트가 존재하나 확인
	SCRIPT_TYPE ScriptsType = pScripttype->GetScriptType();
	for (auto& pair : pvecScripts)
	{
		// 같은 스크립트가 존재하면 만든 스크립트 삭제하고 끝냄
		SCRIPT_TYPE vecScriptsType = pair->GetScriptType();
		if (ScriptsType == vecScriptsType)
		{
			delete pScripttype;
			return;
		}
	}

	m_TargetObject->AddComponent(pScripttype);

	SetTargetObject(m_TargetObject);
}

void Inspector::AddScriptCliked(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	string* pStr = (string*)_SelectString;

	if (*pStr == "None")
	{
		return;
	}

	CScript* pScripttype = GameplayManager::GetScript(wstring(pStr->begin(), pStr->end()));

	const vector<CScript*>& pvecScripts = m_TargetObject->GetScripts();

	// 같은 스크립트가 존재하나 확인
	SCRIPT_TYPE ScriptsType = pScripttype->GetScriptType();
	for (auto& pair : pvecScripts)
	{
		// 같은 스크립트가 존재하면 만든 스크립트 삭제하고 끝냄
		SCRIPT_TYPE vecScriptsType = pair->GetScriptType();
		if (ScriptsType == vecScriptsType)
		{
			delete pScripttype;
			return;
		}
	}

	m_TargetObject->AddComponent(pScripttype);

	SetTargetObject(m_TargetObject);
}

void Inspector::DeleteComponent(COMPONENT_TYPE _Type)
{
	m_TargetObject->DeleteComponent(_Type);

	SetTargetObject(m_TargetObject);
}


void Inspector::CreateComponentUI()
{
	// InspectorUI 의 자식UI 추가
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::TRANSFORM)] = static_cast<ComponentUI*>(AddChildUI(
		new TransformUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::TRANSFORM)]->SetChildSize(ImVec2(0.f, 240.f));

	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::COLLIDER2D)] = static_cast<ComponentUI*>(
		AddChildUI(new Collider2DUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::COLLIDER2D)]->SetChildSize(ImVec2(0.f, 120.f));

	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::COLLIDER3D)] = static_cast<ComponentUI*>(
		AddChildUI(new Collider3DUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::COLLIDER3D)]->SetChildSize(ImVec2(0.f, 150.f));

	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::COLLIDERRAY)] = static_cast<ComponentUI*>(
		AddChildUI(new ColliderRayUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::COLLIDERRAY)]->SetChildSize(ImVec2(0.f, 200.f));

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

	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::DECAL)] = static_cast<ComponentUI*>(
		AddChildUI(new DecalUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::DECAL)]->SetChildSize(ImVec2(0.f, 165.f));

	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::PARTICLE_SYSTEM)] = static_cast<ComponentUI*>(
		AddChildUI(new ParticleUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::PARTICLE_SYSTEM)]->SetChildSize(ImVec2(0.f, 300.f));

	// FIXME : UI / UIRender ui 추가
	/*m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::UI)] = static_cast<ComponentUI*>(
		AddChildUI(new UIUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::UI)]->SetChildSize(ImVec2(0.f, 165.f));

	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::UIRENDER)] = static_cast<ComponentUI*>(
		AddChildUI(new UIRenderUI));
	m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::UIRENDER)]->SetChildSize(ImVec2(0.f, 165.f));*/
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
	m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::ANIMATION)] = new AnimationUI;

	// FIXME : 애셋 UI 추가
	m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::SKELETON)] = new ComputeShaderUI;


	for (UINT i = 0; i < static_cast<UINT>(ASSET_TYPE::END); ++i)
	{
		AddChildUI(m_arrAssetUI[i]);
	}
}
