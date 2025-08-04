#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Grenade_Throw_Low.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

Player_Grenade_Throw_Low::Player_Grenade_Throw_Low()
	: PlayerState(L"Player_Grenade_Throw_Low")
{
	SetCanExitDuringAnimation(false);
}

Player_Grenade_Throw_Low::~Player_Grenade_Throw_Low()
{
}

void Player_Grenade_Throw_Low::Enter_Override()
{
	MOTION_STATE eCurState = GetPlayerScript()->GetMotionState();

	m_Delay = 0.07f;
	switch (eCurState)
	{
	case MOTION_STATE::STAND:
		m_ClipName = L"Animation\\Armature_toss_grenade_low.anim";
		break;
	case MOTION_STATE::CROUCH:
		m_ClipName = L"Animation\\Armature_toss_grenade_low.anim";
		break;
	case MOTION_STATE::PRONE:
		m_ClipName = L"Animation\\Armature_prone_toss_grenade_test.anim";
		break;
	}

	// 애니메이션 실행
	AdjustAnim();
}

void Player_Grenade_Throw_Low::FinalTick_Override()
{
}

void Player_Grenade_Throw_Low::Exit()
{
}
