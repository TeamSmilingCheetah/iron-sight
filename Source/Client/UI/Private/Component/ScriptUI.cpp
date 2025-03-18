#include "pch.h"
#include "Client/UI/Public/Component/ScriptUI.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Client/UI/Public/Editor/ParamUI.h"
#include "Game/System/Public/GameplayManager.h"

ScriptUI::ScriptUI()
	: ComponentUI("ScriptUI", COMPONENT_TYPE::SCRIPT)
{
}

ScriptUI::~ScriptUI()
{
}

void ScriptUI::Render_Update()
{
	string ScriptName = WStringToString(GameplayManager::GetScriptName(m_TargetScript));
	ComponentTitle(ScriptName.c_str());


	const vector<tScriptParam>& vecParam = m_TargetScript->GetScriptParam();

	for (size_t i = 0; i < vecParam.size(); ++i)
	{
		switch (vecParam[i].Param)
		{
		case SCRIPT_PARAM::INT:
			ParamUI::Param_Int(vecParam[i].Desc, reinterpret_cast<int*>(vecParam[i].pData), false);
			break;
		case SCRIPT_PARAM::FLOAT:
			ParamUI::Param_Float(vecParam[i].Desc, reinterpret_cast<float*>(vecParam[i].pData), false);
			break;
		case SCRIPT_PARAM::VEC2:
			ParamUI::Param_Vec2(vecParam[i].Desc, reinterpret_cast<Vec2*>(vecParam[i].pData), false);
			break;
		case SCRIPT_PARAM::VEC4:
			ParamUI::Param_Vec4(vecParam[i].Desc, reinterpret_cast<Vec4*>(vecParam[i].pData), false);
			break;
		case SCRIPT_PARAM::TEXTURE:
			ParamUI::Param_Tex(vecParam[i].Desc, *reinterpret_cast<Ptr<CTexture>*>(vecParam[i].pData));
			break;
		case SCRIPT_PARAM::PREFAB:
			ParamUI::Param_Prefab(vecParam[i].Desc, *reinterpret_cast<Ptr<CPrefab>*>(vecParam[i].pData));
			break;
		case SCRIPT_PARAM::GAMEOBJECT:
			ParamUI::Param_GameObject(vecParam[i].Desc, reinterpret_cast<CGameObject**>(vecParam[i].pData));
			break;
		}
	}
}

void ScriptUI::SetScript(CScript* _Script)
{
	m_TargetScript = _Script;

	if (nullptr == m_TargetScript)
	{
		SetTargetObject(nullptr);
		SetActive(false);
	}
	else
	{
		SetTargetObject(m_TargetScript->GetOwner());
		SetActive(true);
	}
}
