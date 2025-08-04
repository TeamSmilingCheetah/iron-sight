#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Bandage.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

Player_Bandage::Player_Bandage()
	: PlayerState(L"Player_Bandage")
{
	SetCanExitDuringAnimation(false);
}

Player_Bandage::~Player_Bandage()
{
}

void Player_Bandage::Enter_Override()
{
	//MOTION_STATE eCurState = GetPlayerScript()->GetMotionState();

	//m_Delay = 0.07f;
	//switch (eCurState)
	//{
	//case MOTION_STATE::STAND:
	//	m_ClipName = L"Animation\\Armature_reloading.anim";
	//	break;
	//case MOTION_STATE::CROUCH:
	//	m_ClipName = L"Animation\\Armature_reloading.anim";
	//	break;
	//case MOTION_STATE::PRONE:
	//	m_ClipName = L"Animation\\Armature_prone_reloading.anim";
	//	break;
	//}

	//// 애니메이션 실행
	//AdjustAnim();
}

void Player_Bandage::FinalTick_Override()
{
}

void Player_Bandage::Exit()
{
}



