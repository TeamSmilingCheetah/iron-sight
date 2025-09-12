#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Grenade_Prepare_Low.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"



Player_Grenade_Prepare_Low::Player_Grenade_Prepare_Low()
	: PlayerState(L"Player_Grenade_Prepare_Low")
	, m_bInputThrow(false)
	, m_bLBTN(false)
{
	SetCanExitDuringAnimation(false);
}

Player_Grenade_Prepare_Low::~Player_Grenade_Prepare_Low()
{
}

void Player_Grenade_Prepare_Low::Enter_Override()
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

void Player_Grenade_Prepare_Low::FinalTick_Override()
{
	if (KEY_RELEASED(KEY::RBTN))
	{
		m_bInputThrow = true;
		m_bLBTN = false;
	}
	GetPlayerScript()->ProgressThrowPrepareState(m_bInputThrow, m_bLBTN);
}

void Player_Grenade_Prepare_Low::Exit_Override()
{
	m_bInputThrow = false;
	m_bLBTN = false;
	GetPlayerScript()->ExitThrowPrepareState();
}

