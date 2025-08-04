#include "pch.h"
#include "Game/Gameplay/State/Public/Player_FirstAidKit.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

Player_FirstAidKit::Player_FirstAidKit()
	: PlayerState(L"Player_FirstAidKit")
{
	SetCanExitDuringAnimation(false);
}

Player_FirstAidKit::~Player_FirstAidKit()
{
}

void Player_FirstAidKit::Enter_Override()
{
}

void Player_FirstAidKit::FinalTick_Override()
{
}

void Player_FirstAidKit::Exit()
{
}


