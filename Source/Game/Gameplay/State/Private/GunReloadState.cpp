#include "pch.h"
#include "Game/Gameplay/State/Public/GunReloadState.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


GunReloadState::GunReloadState()
	: CState(ACTION_STATE::GUN_RELOAD)
{
}

GunReloadState::~GunReloadState()
{
}


void GunReloadState::Enter()
{
}

void GunReloadState::FinalTick()
{
}

void GunReloadState::Exit()
{
}


