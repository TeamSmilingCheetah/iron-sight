#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Gun_Fire.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

Player_Gun_Fire::Player_Gun_Fire()
	: PlayerState(L"Player_Gun_Fire")
{
	SetCanExitDuringAnimation(true);
}

Player_Gun_Fire::~Player_Gun_Fire()
{
}

void Player_Gun_Fire::Enter_Override()
{
	MOTION_STATE eCurState = GetPlayerScript()->GetMotionState();

	m_Delay = 0.07f;
	switch (eCurState)
	{
	case MOTION_STATE::STAND:
		m_ClipName = L"Animation\\Armature_firing_rifle.anim";
		break;
	case MOTION_STATE::CROUCH:
		m_ClipName = L"Animation\\Armature_firing_rifle.anim";
		break;
	case MOTION_STATE::PRONE:
		m_ClipName = L"Animation\\Armature_prone_firing_rifle.anim";
		break;
	}

	// 애니메이션 실행
	AdjustAnim();
}

void Player_Gun_Fire::FinalTick_Override()
{
}

void Player_Gun_Fire::Exit()
{
}

