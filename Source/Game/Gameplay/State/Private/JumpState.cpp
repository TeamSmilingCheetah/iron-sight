#include "pch.h"
#include "Game/Gameplay/State/Public/JumpState.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


JumpState::JumpState()
	: CState(ACTION_STATE::JUMP)
{
}

JumpState::~JumpState()
{
}

void JumpState::Enter()
{
}

void JumpState::FinalTick()
{
}

void JumpState::Exit()
{
}
