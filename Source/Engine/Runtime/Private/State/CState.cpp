#include "pch.h"
#include "Engine/Runtime/Public/State/CState.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

CState::CState(const wstring& _Name)
	: m_Owner(nullptr)
{
	SetName(_Name);
}

CState::~CState()
{
}
