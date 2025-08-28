#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Jump_Loop.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"

Player_Jump_Loop::Player_Jump_Loop()
	: PlayerState(L"Player_Jump_Loop")
{
	SetCanExitDuringAnimation(true);
}

Player_Jump_Loop::~Player_Jump_Loop()
{
}

void Player_Jump_Loop::Enter_Override()
{
	m_Delay = 0.02f;

	// 점프 진입 애니메이션
	m_ClipName = L"Animation\\Armature_jump_loop.anim";

	AdjustAnim();
}

void Player_Jump_Loop::FinalTick_Override()
{
}

void Player_Jump_Loop::Exit_Override()
{
}
