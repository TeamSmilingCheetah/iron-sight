#include "pch.h"
#include "Game/Gameplay/State/Public/Player_PainKiller.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


Player_PainKiller::Player_PainKiller()
	: PlayerState(L"Player_PainKiller")
{
}

Player_PainKiller::~Player_PainKiller()
{
}


void Player_PainKiller::Enter_Override()
{
}

void Player_PainKiller::FinalTick_Override()
{
}

void Player_PainKiller::Exit()
{
}

