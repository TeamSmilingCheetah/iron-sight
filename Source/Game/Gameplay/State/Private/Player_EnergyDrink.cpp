#include "pch.h"
#include "Game/Gameplay/State/Public/Player_EnergyDrink.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

Player_EnergyDrink::Player_EnergyDrink()
	: PlayerState(L"Player_EnergyDrink")
{
	SetCanExitDuringAnimation(false);
}

Player_EnergyDrink::~Player_EnergyDrink()
{
}

void Player_EnergyDrink::Enter_Override()
{
	MOTION_STATE eCurState = GetPlayerScript()->GetMotionState();

	m_Delay = 0.07f;
	switch (eCurState)
	{
	case MOTION_STATE::STAND:
		m_ClipName = L"Animation\\Armature_stand_energy_drink.anim";
		break;
	case MOTION_STATE::CROUCH:
		m_ClipName = L"Animation\\Armature_stand_energy_drink.anim";
		break;
	case MOTION_STATE::PRONE:
		m_ClipName = L"Animation\\Armature_prone_energy_drink.anim";
		break;
	}

	// 애니메이션 실행
	AdjustAnim();
}

void Player_EnergyDrink::FinalTick_Override()
{
}

void Player_EnergyDrink::Exit()
{
}


