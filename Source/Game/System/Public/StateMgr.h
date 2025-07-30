#pragma once

class CState;

class StateMgr
{
public:
	static void GetStateInfo(vector<wstring>& _vec);
	static CState* GetState(const wstring& _strStateName);
	static CState* GetState(UINT _iScriptType);
	static const wchar_t* GetStateName(CState* _pState);
};
