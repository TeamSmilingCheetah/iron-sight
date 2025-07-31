#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Dead.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

Player_Dead::Player_Dead()
	: PlayerState(L"Player_Dead")
{
}

Player_Dead::~Player_Dead()
{
}

void Player_Dead::Enter_Override()
{
	MOTION_STATE eCurState = GetPlayerScript()->GetMotionState();

	m_Delay = 1.f;
	switch (eCurState)
	{
	case MOTION_STATE::STAND:
		m_ClipName = L"Animation\\Armature_death_from_the_front.anim";
		break;
	case MOTION_STATE::CROUCH:
		m_ClipName = L"Animation\\Armature_death_from_the_front.anim";
		break;
	case MOTION_STATE::PRONE:
		m_ClipName = L"Animation\\Armature_prone_death.anim";
		break;
	default:
		break;
	}

	// 애니메이션 실행
	AdjustAnim();
}

void Player_Dead::FinalTick_Override()
{
}

void Player_Dead::Exit()
{
}



