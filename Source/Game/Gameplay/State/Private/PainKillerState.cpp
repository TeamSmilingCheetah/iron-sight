#include "pch.h"
#include "Game/Gameplay/State/Public/PainKillerState.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


PainKillerState::PainKillerState()
	: CState(ACTION_STATE::PAIN_KILLER)
{
}

PainKillerState::~PainKillerState()
{
}


void PainKillerState::Enter()
{
}

void PainKillerState::FinalTick()
{
}

void PainKillerState::Exit()
{
}

