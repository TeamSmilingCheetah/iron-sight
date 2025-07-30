#include "pch.h"
#include "Game/Gameplay/State/Public/DeadState.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


DeadState::DeadState()
	: CState(ACTION_STATE::DEAD)
{
}

DeadState::~DeadState()
{
}

void DeadState::Enter()
{
}

void DeadState::FinalTick()
{
}

void DeadState::Exit()
{
}



