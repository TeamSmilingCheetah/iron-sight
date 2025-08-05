#pragma once
#include "Common/singleton.h"

class CScript;

class CScriptMgr
	: public singleton<CScriptMgr>
{
	SINGLE(CScriptMgr)

private:
	map<wstring, function<CScript*()>>		m_mapReturnFunc;
	array<wstring, int(SCRIPT_TYPE::NONE) + 1>	m_arrScriptName;	// SCRIPT_TYPE to wstring

public:
	CScript* GetScript(const wstring& _ScriptName);
	CScript* GetScript(SCRIPT_TYPE _Type);
	const wstring& GetScriptName(SCRIPT_TYPE _Type) { return m_arrScriptName[int(_Type)]; }
	void GetScriptNamesInVector(vector<wstring>& _Container);

	void RegisterScript(const wstring& _Name, function<CScript* ()> _ReturnFunc);
};
