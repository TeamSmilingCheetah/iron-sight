#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Grenade_Prepare_High.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"



Player_Grenade_Prepare_High::Player_Grenade_Prepare_High()
	: PlayerState(L"Player_Grenade_Prepare_High")
	, m_bInputThrow(false)
	, m_bLBTN(false)
{
	SetCanExitDuringAnimation(false);
}

Player_Grenade_Prepare_High::~Player_Grenade_Prepare_High()
{
}

void Player_Grenade_Prepare_High::Enter_Override()
{
	MOTION_STATE eCurState = GetPlayerScript()->GetMotionState();

	m_Delay = 0.07f;
	switch (eCurState)
	{
	case MOTION_STATE::STAND:
		m_ClipName = L"Animation\\Armature_stand_grenade_prepare.anim";
		break;
	case MOTION_STATE::CROUCH:
		m_ClipName = L"Animation\\Armature_stand_grenade_prepare.anim";
		break;
	case MOTION_STATE::PRONE:
		m_ClipName = L"Animation\\Armature_prone_grenade_prepare.anim";
		break;
	}

	// 애니메이션 실행
	AdjustAnim();
}

void Player_Grenade_Prepare_High::FinalTick_Override()
{
	if (KEY_RELEASED(KEY::LBTN))
	{
		m_bInputThrow = true;
		m_bLBTN = true;
	}
	GetPlayerScript()->ProgressThrowPrepareState(m_bInputThrow, m_bLBTN);
}

void Player_Grenade_Prepare_High::Exit_Override()
{
	m_bInputThrow = false;
	m_bLBTN = false;
	GetPlayerScript()->ExitThrowPrepareState();
}

