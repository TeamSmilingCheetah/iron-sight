#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Heal.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"

Player_Heal::Player_Heal()
	: PlayerState(L"Player_Heal")
{
	// TODO(Ssio) : false로 바꾸고, 애니메이션과 heal 시간 동기화 필요
	SetCanExitDuringAnimation(false);
}

Player_Heal::~Player_Heal()
{
}


void Player_Heal::Enter_Override()
{
	MOTION_STATE eCurState = GetPlayerScript()->GetMotionState();
	ITEM_TYPE eHealItem = GetPlayerScript()->GetHealType();

	m_ClipName = L"Animation\\Armature_";
	m_Delay = 0.07f;

	// 자세에 따라
	switch (eCurState)
	{
	case MOTION_STATE::STAND:
		m_ClipName += L"stand_";
		break;
	case MOTION_STATE::CROUCH:
		m_ClipName += L"stand_";
		break;
	case MOTION_STATE::PRONE:
		m_ClipName += L"prone_";
		break;
	}

	// Heal 아이템에 따라
	switch (eHealItem)
	{
	case ITEM_TYPE::BANDAGE:
		// TODO(Ssio): Animation Miss
		assert(!"Animation File Doesn't Exist");
		m_ClipName += L"first_aid_kit.anim";
		break;
	case ITEM_TYPE::MED_KIT:
		// TODO(Ssio): Animation Miss
		assert(!"Animation File Doesn't Exist");
		m_ClipName += L"first_aid_kit.anim";
		break;
	case ITEM_TYPE::FIRST_AID_KIT:
		m_ClipName += L"first_aid_kit.anim";
		break;
	case ITEM_TYPE::ENERGY_DRINK:
		m_ClipName += L"energy_drink.anim";
		break;
	case ITEM_TYPE::PAIN_KILLER:
		m_ClipName += L"pain_killer.anim";
		break;
	case ITEM_TYPE::ADRENALINE_SYRINGE:
		m_ClipName += L"adrenaline.anim";
		break;
	default:
		assert(!"Not Heal Item");
	}

	// 애니메이션 실행
	AdjustAnim();
}

// Heal Item 사용 시간 계산 로직만 포함
void Player_Heal::FinalTick_Override()
{
	GetPlayerScript()->ProgressHealState();
}

void Player_Heal::Exit()
{
	
}

