#include "pch.h"
#include "Engine/System/Public/Manager/CStateMgr.h"
#include "Engine/Runtime/Public/State/CState.h"

CStateMgr::CStateMgr()
{

}

CStateMgr::~CStateMgr()
{
}

CState* CStateMgr::GetState(const wstring& _StateName)
{
	auto iter = m_mapReturnFunc.find(_StateName);
	if (iter == m_mapReturnFunc.end())
		return nullptr;

	return (iter->second)();
}

void CStateMgr::RegisterState(const wstring& _Name, function<CState* ()> _ReturnFunc)
{
	m_mapReturnFunc.emplace(_Name, _ReturnFunc);
}
