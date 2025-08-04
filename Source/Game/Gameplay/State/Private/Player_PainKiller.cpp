#include "pch.h"
#include "Game/Gameplay/State/Public/Player_PainKiller.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

Player_PainKiller::Player_PainKiller()
	: PlayerState(L"Player_PainKiller")
{
	SetCanExitDuringAnimation(false);
}

Player_PainKiller::~Player_PainKiller()
{
}


void Player_PainKiller::Enter_Override()
{
	MOTION_STATE eCurState = GetPlayerScript()->GetMotionState();

	m_Delay = 0.07f;
	switch (eCurState)
	{
	case MOTION_STATE::STAND:
		m_ClipName = L"Animation\\Armature_stand_pain_killer.anim";
		break;
	case MOTION_STATE::CROUCH:
		m_ClipName = L"Animation\\Armature_stand_pain_killer.anim";
		break;
	case MOTION_STATE::PRONE:
		m_ClipName = L"Animation\\Armature_prone_pain_killer.anim";
		break;
	}

	// 애니메이션 실행
	AdjustAnim();
}

void Player_PainKiller::FinalTick_Override()
{
}

void Player_PainKiller::Exit()
{
}

