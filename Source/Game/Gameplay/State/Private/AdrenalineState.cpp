#include "pch.h"
#include "Game/Gameplay/State/Public/AdrenalineState.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

AdrenalineState::AdrenalineState()
	: CState(ACTION_STATE::ADRENALINE_SYRINGE)
{
}

AdrenalineState::~AdrenalineState()
{
}


void AdrenalineState::Enter()
{
}

void AdrenalineState::FinalTick()
{
}

void AdrenalineState::Exit()
{
}

