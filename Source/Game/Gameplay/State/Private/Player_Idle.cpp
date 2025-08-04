#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Idle.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

Player_Idle::Player_Idle()
	: PlayerState(L"Player_Idle")
{
	SetCanExitDuringAnimation(true);
}

Player_Idle::~Player_Idle()
{
}

void Player_Idle::Enter_Override()
{
	MOTION_STATE eCurState = GetPlayerScript()->GetMotionState();

	m_Delay = 0.07f;
	switch (eCurState)
	{
	case MOTION_STATE::STAND:
		m_ClipName = L"Animation\\Armature_idle.anim";
		break;
	case MOTION_STATE::CROUCH:
		m_ClipName = L"Animation\\Armature_idle_crouching.anim";
		break;
	case MOTION_STATE::PRONE:
		m_ClipName = L"Animation\\Armature_prone_idle.anim";
		break;
	}

	// 애니메이션 실행
	AdjustAnim();
}

void Player_Idle::FinalTick_Override()
{
}

void Player_Idle::Exit()
{
}

