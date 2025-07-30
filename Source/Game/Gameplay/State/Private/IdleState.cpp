#include "pch.h"
#include "Game/Gameplay/State/Public/IdleState.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


IdleState::IdleState()
	: CState(ACTION_STATE::NONE)
{
}

IdleState::~IdleState()
{
}

void IdleState::Enter()
{
}

void IdleState::FinalTick()
{
}

void IdleState::Exit()
{
}

