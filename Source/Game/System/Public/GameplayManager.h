#pragma once

enum SCRIPT_TYPE
{
	CAMERASCRIPT,
	MISSILESCRIPT,
	PLAYERSCRIPT,
	GUNSCRIPT,
};

class CScript;

class GameplayManager
{
public:
	static void GetScriptInfo(vector<wstring>& _vec);
	static CScript* GetScript(const wstring& _strScriptName);
	static CScript* GetScript(UINT _iScriptType);
	static const wchar_t* GetScriptName(CScript* _pScript);
};
