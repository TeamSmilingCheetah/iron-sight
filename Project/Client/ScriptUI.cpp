#include "pch.h"
#include "ScriptUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CScript.h>
#include <Scripts/CScriptMgr.h>

#include "ParamUI.h"

ScriptUI::ScriptUI()
    : ComponentUI("ScriptUI", COMPONENT_TYPE::SCRIPT)
{
}

ScriptUI::~ScriptUI()
{
}

void ScriptUI::Render_Update()
{
    wstring wScriptName = CScriptMgr::GetScriptName(m_TargetScript);
    auto ScriptName = string(wScriptName.begin(), wScriptName.end());
    ComponentTitle(ScriptName.c_str());


    const vector<tScriptParam>& vecParam = m_TargetScript->GetScriptParam();

    for (size_t i = 0; i < vecParam.size(); ++i)
    {
        switch (vecParam[i].Param)
        {
        case SCRIPT_PARAM::INT:
            ParamUI::Param_Int(vecParam[i].Desc, static_cast<int*>(vecParam[i].pData), false);
            break;
        case SCRIPT_PARAM::FLOAT:
            ParamUI::Param_Float(vecParam[i].Desc, static_cast<float*>(vecParam[i].pData), false);
            break;
        case SCRIPT_PARAM::VEC2:
            ParamUI::Param_Vec2(vecParam[i].Desc, static_cast<Vec2*>(vecParam[i].pData), false);
            break;
        case SCRIPT_PARAM::VEC4:
            ParamUI::Param_Vec4(vecParam[i].Desc, static_cast<Vec4*>(vecParam[i].pData), false);
            break;
        case SCRIPT_PARAM::TEXTURE:
            ParamUI::Param_Tex(vecParam[i].Desc, *static_cast<Ptr<CTexture>*>(vecParam[i].pData));
            break;
        case SCRIPT_PARAM::PREFAB:
            ParamUI::Param_Prefab(vecParam[i].Desc, *static_cast<Ptr<CPrefab>*>(vecParam[i].pData));
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
