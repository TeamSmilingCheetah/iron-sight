#include "pch.h"
#include "Game/Gameplay/State/Public/BandageState.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


BandageState::BandageState()
	: CState(ACTION_STATE::BANDAGE)
{
}

BandageState::~BandageState()
{
}

void BandageState::Enter()
{
}

void BandageState::FinalTick()
{
}

void BandageState::Exit()
{
}



