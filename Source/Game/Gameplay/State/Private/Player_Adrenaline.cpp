#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Adrenaline.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

Player_Adrenaline::Player_Adrenaline()
	: PlayerState(L"Player_Adrenaline")
{
	SetCanExitDuringAnimation(false);
}

Player_Adrenaline::~Player_Adrenaline()
{
}


void Player_Adrenaline::Enter_Override()
{
}

void Player_Adrenaline::FinalTick_Override()
{
}

void Player_Adrenaline::Exit()
{
}

