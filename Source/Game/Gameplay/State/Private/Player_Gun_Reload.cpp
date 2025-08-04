#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Gun_Reload.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


Player_Gun_Reload::Player_Gun_Reload()
	: PlayerState(L"Player_Gun_Reload")
{
	SetCanExitDuringAnimation(false);
}

Player_Gun_Reload::~Player_Gun_Reload()
{
}


void Player_Gun_Reload::Enter_Override()
{
}

void Player_Gun_Reload::FinalTick_Override()
{
}

void Player_Gun_Reload::Exit()
{
}


