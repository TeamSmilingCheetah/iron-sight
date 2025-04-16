#pragma once
#include "Client/UI/Public/Editor/EditorUI.h"
#include "Common/Ptr.h"
#include "Game/System/Public/GameplayManager.h"

class CAsset;
class CGameObject;
class ComponentUI;
class ScriptUI;
class AssetUI;

class Inspector :
	public EditorUI
{
	CGameObject* m_TargetObject;
	ComponentUI* m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::END)];
	vector<ScriptUI*> m_vecScriptUI;

	Ptr<CAsset> m_TargetAsset;
	AssetUI* m_arrAssetUI[static_cast<UINT>(END)];

	int                 m_ComponentListIdx;

public:
	void SetTargetObject(CGameObject* _Target);
	void SetTargetAsset(Ptr<CAsset> _Asset);

	void Render_Update() override;

	void AddComponent(COMPONENT_TYPE _Type);
	void AddScript(SCRIPT_TYPE _Type);
	void AddScriptCliked(DWORD_PTR _ListUI, DWORD_PTR _SelectString);
	void DeleteComponent(COMPONENT_TYPE _Type);

private:
	void CreateComponentUI();
	void CreateAssetUI();

public:
	Inspector();
	~Inspector() override;
};
