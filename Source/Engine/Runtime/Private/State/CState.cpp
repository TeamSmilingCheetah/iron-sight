#include "pch.h"

#include "Engine/Runtime/Public/State/CState.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

CState::CState(ACTION_STATE _State)
	: m_Owner(nullptr)
	, m_OwnState(_State)
{

}

CState::~CState()
{
}
