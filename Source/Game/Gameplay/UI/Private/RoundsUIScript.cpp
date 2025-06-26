#include "pch.h"

#include "Game/Gameplay/UI/Public/RoundsUIScript.h"
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Weapon/Public/ThrowableController.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/GamePlay/Inventory/Public/InventoryController.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Game/Gameplay/Inventory/Public/Item.h"

RoundsUIScript::RoundsUIScript()
	: CScript(SCRIPT_TYPE::ROUNDSSUI)
	, m_Player(nullptr)
	, m_PlayerScript(nullptr)
	, m_PlayerWeapon(nullptr)
	, m_InvetoryScript(nullptr)
{
}

RoundsUIScript::~RoundsUIScript()
{
}


void RoundsUIScript::Begin()
{
	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));
	m_InvetoryScript = static_cast<InventoryController*>(GetScriptWithType(m_Player, SCRIPT_TYPE::INVENTORYSCRIPT));
}

void RoundsUIScript::Tick()
{
	m_PlayerWeapon = m_InvetoryScript->GetCurWeapon();

	// 무기 상태가 바뀌면 UI초기화
	if (m_InvetoryScript->IsChange())
	{
		UI()->ClearText();
		m_InvetoryScript->OffChange();
	}

	// 무기가 없다면 Tick 반환
	if (m_PlayerWeapon == nullptr)
	{
		UI()->ClearText();
		return;
	}

	GunController* pGunScript = static_cast<GunController*>(GetScriptWithType(m_PlayerWeapon, SCRIPT_TYPE::GUNSCRIPT));


	// 투척무기
	if (pGunScript == nullptr)
	{
		// 현재 들고 있는 투척무기 종류의 개수를 구한다.
		ITEM_TYPE type = static_cast<ItemScript*>(GetScriptWithType(m_PlayerWeapon, SCRIPT_TYPE::ITEMSCRIPT))->GetItemType();
		int iCurCounts = m_InvetoryScript->GetItemCount(type);
		wstring wCurCounts = to_wstring(iCurCounts);


		// 만약 현재 UI TEXT가 비어있는 상태라면 만들어준다.
		if (UI()->GetTextvecSize() == 0)
		{
			UI()->AddText(L"", 11.f, 4.f, 25, FONT_RGBA(255, 255, 255, 255));
		}

		// 만들어진 UI들을 수정하여 정보를 표시한다.
		UI()->ModifyText(0, wCurCounts, 45.f, 4.f, 30, FONT_RGBA(255, 255, 255, 255));
	}

	// 총기
	else
	{
		int iCurRounds = pGunScript->GetCurRound();
		ITEM_TYPE RoundType = pGunScript->GetRoundType();
		int iLeftRounds = m_InvetoryScript->GetItemCount(RoundType);
		bool bAuto = pGunScript->IsAuto();

		wstring wCurRounds = to_wstring(iCurRounds);
		wstring wLeftRounds = to_wstring(iLeftRounds);

		// 만약 현재 UI TEXT가 비어있는 상태라면 만들어준다.
		if (UI()->GetTextvecSize() == 0)
		{
			UI()->AddText(L"", 11.f, 4.f, 25, FONT_RGBA(255, 255, 255, 255));
			UI()->AddText(L"", 11.f, 4.f, 25, FONT_RGBA(255, 255, 255, 255));
			UI()->AddText(L"", 30.f, 4.f, 30, FONT_RGBA(255, 255, 255, 255));
		}

		// 만들어진 UI들을 수정하여 정보를 표시한다.
		UI()->ModifyText(0, wCurRounds, 11.f, 4.f, 30, FONT_RGBA(255, 255, 255, 255));
		UI()->ModifyText(1, wLeftRounds, 50.f, 14.f, 20, FONT_RGBA(128, 128, 128, 155));
		if (bAuto)
		{
			UI()->ModifyText(2, L"연발", 85.f, 0.f, 30, FONT_RGBA(255, 255, 255, 255));
		}
		else
		{
			UI()->ModifyText(2, L"단발", 85.f, 0.f, 30, FONT_RGBA(255, 255, 255, 255));
		}
	}
}

void RoundsUIScript::SaveComponent(FILE* _File)
{
}

void RoundsUIScript::LoadComponent(FILE* _File)
{
}


