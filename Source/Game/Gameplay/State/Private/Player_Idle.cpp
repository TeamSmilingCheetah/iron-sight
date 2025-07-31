#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Idle.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


Player_Idle::Player_Idle()
	: PlayerState(L"Player_Idle")
{
}

Player_Idle::~Player_Idle()
{
}

void Player_Idle::Enter_Override()
{
}

void Player_Idle::FinalTick_Override()
{
}

void Player_Idle::Exit()
{
}

