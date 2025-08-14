#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Jump_Up.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"

Player_Jump_Up::Player_Jump_Up()
	: PlayerState(L"Player_Jump_Up")
{
	SetCanExitDuringAnimation(true);
}

Player_Jump_Up::~Player_Jump_Up()
{
}

void Player_Jump_Up::Enter_Override()
{
	m_Delay = 0.02f;

	// 점프 진입 애니메이션
	m_ClipName = L"Animation\\Armature_jump_up.anim";

	AdjustAnim();
}

void Player_Jump_Up::FinalTick_Override()
{
	PlayerCharacter* playerScript = GetPlayerScript();
}

void Player_Jump_Up::Exit_Override()
{
}
