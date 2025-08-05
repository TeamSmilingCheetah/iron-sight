#pragma once
#include "Common/singleton.h"

class CState;

class CStateMgr
	: public singleton<CStateMgr>
{
	SINGLE(CStateMgr)

private:
	map<wstring, function<CState* ()>> m_mapReturnFunc;

public:
	CState* GetState(const wstring& _StateName);

	void RegisterState(const wstring& _Name, function<CState* ()> _ReturnFunc);
};
