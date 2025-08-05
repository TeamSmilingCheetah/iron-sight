#include "pch.h"
#include "Engine/System/Public/Manager/CScriptMgr.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"

CScriptMgr::CScriptMgr()
{
}

CScriptMgr::~CScriptMgr()
{
}

CScript* CScriptMgr::GetScript(const wstring& _ScriptName)
{
	auto iter = m_mapReturnFunc.find(_ScriptName);
	if (iter == m_mapReturnFunc.end())
		return nullptr;

	return (iter->second)();
}

CScript* CScriptMgr::GetScript(SCRIPT_TYPE _Type)
{
	return GetScript(m_arrScriptName[static_cast<int>(_Type)]);
}

void CScriptMgr::GetScriptNamesInVector(vector<wstring>& _Container)
{
	for (int i = 0; static_cast<int>(SCRIPT_TYPE::NONE); ++i)
	{
		if (m_arrScriptName[i] == L"")
			continue;

		_Container.push_back(m_arrScriptName[i]);
	}
}

void CScriptMgr::RegisterScript(const wstring& _Name, function<CScript* ()> _ReturnFunc)
{
	m_mapReturnFunc.emplace(_Name, _ReturnFunc);

	CScript* pScript = _ReturnFunc();

	m_arrScriptName[static_cast<int>(pScript->GetScriptType())] = _Name;

	delete pScript;
}
