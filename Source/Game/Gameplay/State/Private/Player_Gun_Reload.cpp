#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Gun_Reload.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"

#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

Player_Gun_Reload::Player_Gun_Reload()
	: PlayerState(L"Player_Gun_Reload")
{
	SetCanExitDuringAnimation(false);
}

Player_Gun_Reload::~Player_Gun_Reload()
{
}


void Player_Gun_Reload::Enter_Override()
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

void Player_Gun_Reload::FinalTick_Override()
{
	GetPlayerScript()->ProgressReloadState();

	// TODO : 애니메이션이랑 시간 동기화
}

void Player_Gun_Reload::Exit_Override()
{
	GetPlayerScript()->ExitReloadState();
}


