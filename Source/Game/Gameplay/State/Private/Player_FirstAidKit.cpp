#include "pch.h"
#include "Game/Gameplay/State/Public/Player_FirstAidKit.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

Player_FirstAidKit::Player_FirstAidKit()
	: PlayerState(L"Player_FirstAidKit")
{
	SetCanExitDuringAnimation(false);
}

Player_FirstAidKit::~Player_FirstAidKit()
{
}

void Player_FirstAidKit::Enter_Override()
{
	MOTION_STATE eCurState = GetPlayerScript()->GetMotionState();

	m_Delay = 0.07f;
	switch (eCurState)
	{
	case MOTION_STATE::STAND:
		m_ClipName = L"Animation\\Armature_stand_first_aid_kit.anim";
		break;
	case MOTION_STATE::CROUCH:
		m_ClipName = L"Animation\\Armature_stand_first_aid_kit.anim";
		break;
	case MOTION_STATE::PRONE:
		m_ClipName = L"Animation\\Armature_prone_first_aid_kit.anim";
		break;
	}

	// 애니메이션 실행
	AdjustAnim();
}

void Player_FirstAidKit::FinalTick_Override()
{
}

void Player_FirstAidKit::Exit()
{
}


