#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Dead.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Character/Public/CameraEffect.h"
#include "Game/System/Public/CGameMgr.h"

Player_Dead::Player_Dead()
	: PlayerState(L"Player_Dead")
	, m_AccTime(0.f)
	, m_bCamFlag(false)
{
	SetCanExitDuringAnimation(false);
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

	GetPlayerScript()->EnterDeadState();
}

void Player_Dead::FinalTick_Override()
{
	GetPlayerScript()->ProgressDeadState();

	if (!m_bCamFlag)
	{
		m_AccTime += DT;
	}
	
	// FadeOut 카메라 효과 적용
	if (2.5f < m_AccTime)
	{
		CGameMgr::GetInst()->GetCamEffect()->FadeOut();
		m_bCamFlag = true;
		m_AccTime = 0.f;
	}
}

void Player_Dead::Exit_Override()
{
	m_bCamFlag = false;
}



