#include "pch.h"
#include "Game/Gameplay/State/Public/Player_MedKit.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

Player_MedKit::Player_MedKit()
	: PlayerState(L"Player_MedKit")
{
	SetCanExitDuringAnimation(false);
}

Player_MedKit::~Player_MedKit()
{
}

void Player_MedKit::Enter_Override()
{
}

void Player_MedKit::FinalTick_Override()
{
}

void Player_MedKit::Exit()
{
}


