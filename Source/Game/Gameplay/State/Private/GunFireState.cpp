#include "pch.h"
#include "Game/Gameplay/State/Public/GunFireState.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

GunFireState::GunFireState()
	: CState(ACTION_STATE::GUN_FIRE)
{
}

GunFireState::~GunFireState()
{
}

void GunFireState::Enter()
{
}

void GunFireState::FinalTick()
{
}

void GunFireState::Exit()
{
}

