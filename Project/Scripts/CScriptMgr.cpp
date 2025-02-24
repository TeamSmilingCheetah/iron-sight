#include "pch.h"
#include "CScriptMgr.h"

#include "CCameraScript.h"
#include "CMissileScript.h"
#include "CPlayerScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
    _vec.push_back(L"CCameraScript");
    _vec.push_back(L"CMissileScript");
    _vec.push_back(L"CPlayerScript");
}

CScript* CScriptMgr::GetScript(const wstring& _strScriptName)
{
    if (L"CCameraScript" == _strScriptName)
        return new CCameraScript;
    if (L"CMissileScript" == _strScriptName)
        return new CMissileScript;
    if (L"CPlayerScript" == _strScriptName)
        return new CPlayerScript;
    return nullptr;
}

CScript* CScriptMgr::GetScript(UINT _iScriptType)
{
    switch (_iScriptType)
    {
    case static_cast<UINT>(CAMERASCRIPT):
        return new CCameraScript;
        break;
    case static_cast<UINT>(MISSILESCRIPT):
        return new CMissileScript;
        break;
    case static_cast<UINT>(PLAYERSCRIPT):
        return new CPlayerScript;
        break;
    }
    return nullptr;
}

const wchar_t* CScriptMgr::GetScriptName(CScript* _pScript)
{
    switch (static_cast<SCRIPT_TYPE>(_pScript->GetScriptType()))
    {
    case CAMERASCRIPT:
        return L"CCameraScript";
        break;

    case MISSILESCRIPT:
        return L"CMissileScript";
        break;

    case PLAYERSCRIPT:
        return L"CPlayerScript";
        break;
    }
    return nullptr;
}
