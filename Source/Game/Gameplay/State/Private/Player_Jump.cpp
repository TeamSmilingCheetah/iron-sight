#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Jump.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"


Player_Jump::Player_Jump()
	: PlayerState(L"Player_Jump")
{
	SetCanExitDuringAnimation(false);
}

Player_Jump::~Player_Jump()
{
}

void Player_Jump::Enter_Override()
{
	m_Delay = 0.02f;
	m_ClipName = L"Animation\\Armature_rifle_jump.anim";

	AdjustAnim();
}

void Player_Jump::FinalTick_Override()
{
	if (GetPlayerScript()->IsGround())
	{
		SM()->ChangeState(L"Player_Idle");
	}
}

void Player_Jump::Exit()
{
}
