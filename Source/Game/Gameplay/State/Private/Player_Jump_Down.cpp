#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Jump_Down.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"

Player_Jump_Down::Player_Jump_Down()
	: PlayerState(L"Player_Jump_Down")
{
	SetCanExitDuringAnimation(true);
}

Player_Jump_Down::~Player_Jump_Down()
{
}

void Player_Jump_Down::Enter_Override()
{
	m_Delay = 0.02f;

	// 점프 진입 애니메이션
	m_ClipName = L"Animation\\Armature_jump_down.anim";

	AdjustAnim();
}

void Player_Jump_Down::FinalTick_Override()
{
	if (!SM()->Animator3D()->IsActive())
	{
		SM()->SetChange(L"Player_Idle");
	}
}

void Player_Jump_Down::Exit_Override()
{
}
