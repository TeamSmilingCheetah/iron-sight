#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Dead.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


Player_Dead::Player_Dead()
	: PlayerState(L"Player_Dead")
{
}

Player_Dead::~Player_Dead()
{
}

void Player_Dead::Enter_Override()
{
}

void Player_Dead::FinalTick_Override()
{
}

void Player_Dead::Exit()
{
}



