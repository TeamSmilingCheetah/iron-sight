#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Adrenaline.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

Player_Adrenaline::Player_Adrenaline()
	: PlayerState(L"Player_Adrenaline")
{
	SetCanExitDuringAnimation(false);
}

Player_Adrenaline::~Player_Adrenaline()
{
}


void Player_Adrenaline::Enter_Override()
{
	MOTION_STATE eCurState = GetPlayerScript()->GetMotionState();

	m_Delay = 0.07f;
	switch (eCurState)
	{
	case MOTION_STATE::STAND:
		m_ClipName = L"Animation\\Armature_reloading.anim";
		break;
	case MOTION_STATE::CROUCH:
		m_ClipName = L"Animation\\Armature_reloading.anim";
		break;
	case MOTION_STATE::PRONE:
		m_ClipName = L"Animation\\Armature_prone_reloading.anim";
		break;
	}

	// 애니메이션 실행
	AdjustAnim();

}

void Player_Adrenaline::FinalTick_Override()
{
}

void Player_Adrenaline::Exit()
{
}

