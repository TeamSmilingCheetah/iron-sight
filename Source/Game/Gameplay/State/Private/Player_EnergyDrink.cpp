#include "pch.h"
#include "Game/Gameplay/State/Public/Player_EnergyDrink.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

Player_EnergyDrink::Player_EnergyDrink()
	: PlayerState(L"Player_EnergyDrink")
{
	SetCanExitDuringAnimation(false);
}

Player_EnergyDrink::~Player_EnergyDrink()
{
}

void Player_EnergyDrink::Enter_Override()
{
}

void Player_EnergyDrink::FinalTick_Override()
{
}

void Player_EnergyDrink::Exit()
{
}


