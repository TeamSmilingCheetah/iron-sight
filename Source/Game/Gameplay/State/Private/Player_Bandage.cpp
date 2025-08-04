#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Bandage.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


Player_Bandage::Player_Bandage()
	: PlayerState(L"Player_Bandage")
{
	SetCanExitDuringAnimation(false);
}

Player_Bandage::~Player_Bandage()
{
}

void Player_Bandage::Enter_Override()
{
}

void Player_Bandage::FinalTick_Override()
{
}

void Player_Bandage::Exit()
{
}



