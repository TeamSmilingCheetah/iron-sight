#include "pch.h"
#include "Game/GamePlay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Inventory/Public/Item.h"

#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"

InventoryController::InventoryController()
	: CScript(INVENTORYSCRIPT)
	, m_Player(nullptr)
	, m_PlayerScript(nullptr)
{
}

InventoryController::~InventoryController()
{
}

void InventoryController::SetPlayer(CGameObject* _Player)
{
	m_Player = _Player;
	m_PlayerScript = static_cast<PlayerCharacter*>(m_Player->GetScript(PLAYERSCRIPT));
}

void InventoryController::SyncItemUI(ItemScript* _Item, CGameObject* _ItemUI)
{
	// UI에 Item과 일치하는 값을 설정함

	// UI 구조
	// base : 글씨 및 배경 색상 설정
	// child : 배경 색상 및 아이템 이미지 설정
	_ItemUI->UI()->ClearText();
	_ItemUI->UI()->AddText(_Item->GetOwner()->GetName(), 45.f, 12.f, 12, FONT_RGBA(255, 255, 255, 255));
	wchar_t strCount[4]{};
	swprintf_s(strCount, L"%d", _Item->GetCount());
	_ItemUI->UI()->AddText(strCount, 130.f, 12.f, 12, FONT_RGBA(255, 255, 255, 255));
	_ItemUI->GetChild()[0]->UI()->SetImage(_Item->GetImage());
}

void InventoryController::DisplayUI_Vicinity()
{
	for (int i = 0; i < static_cast<int>(m_vecVicinity.size()); ++i)
	{
		ItemScript* pItem = static_cast<ItemScript*>(m_vecVicinity[i]->GetScript(ITEMSCRIPT));
		assert(pItem != nullptr);

		SyncItemUI(pItem, m_vecVicinityUI[i]);

		// UI의 위치 설정 (인덱스에 따라)
		m_vecVicinityUI[i]->UI()->SetRectPos(0.f, 200.f - 43.f * i);
		m_vecVicinityUI[i]->SetActive(true);
	}

	// 대응되는 범위를 초과하는 UI는 비활성화
	for (int i = static_cast<int>(m_vecVicinity.size()); i < static_cast<int>(m_vecVicinityUI.size()); ++i)
	{
		m_vecVicinityUI[i]->SetActive(false);
	}
}

void InventoryController::DisplayUI_Inventory()
{
	
}

void InventoryController::Init()
{
}

void InventoryController::Begin()
{
}

void InventoryController::Tick()
{
	// F키 눌렀을 때
	if (KEY_TAP(KEY::F))
	{
		// 플레이어가 바라보고 있는 오브젝트
		CGameObject* pTarget = m_PlayerScript->GetRayTarget();

		// 주변에 감지된 아이템 중에서 타겟이 있다면
		for (CGameObject* pItem : m_vecVicinity)
		{
			if (pItem == pTarget)
			{
				// 아이템을 습득함 -> World에서 없애면서 EndOverlap이 호출되면서 ui에 자동으로 반영됨
				m_PlayerScript->AcquireItem(pItem);

				break;
			}
		}
	}
}

void InventoryController::SaveComponent(FILE* _File)
{
}

void InventoryController::LoadComponent(FILE* _File)
{
}

#include <algorithm>

// Compare
bool ItemComp (CGameObject* _lhs, CGameObject* _rhs)
{
	ItemScript* lItem = static_cast<ItemScript*>(_lhs->GetScript(ITEMSCRIPT));
	ItemScript* rItem = static_cast<ItemScript*>(_rhs->GetScript(ITEMSCRIPT));

	ITEM_TYPE lType = lItem->GetItemType();
	ITEM_TYPE rType = rItem->GetItemType();

	// Type에 따라 우선 정렬
	if (lType != rType)
		return lType < rType;

	// Type이 동일한 경우는 개수에 따라 정렬
	return lItem->GetCount() < rItem->GetCount();
}

void InventoryController::BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	// Layer 6번이 Item 레이어임을 가정.
	assert(CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(6)->GetName() == L"Item");

	if (_OtherObject->GetLayerIdx() != 6)
		return;

	// 주변부에 추가함
	m_vecVicinity.push_back(_OtherObject);
	std::sort(m_vecVicinity.begin(), m_vecVicinity.end(), ItemComp);

	// UI에도 반영
	DisplayUI_Vicinity();
}

void InventoryController::Overlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}

void InventoryController::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	// Layer 6번이 Item 레이어임을 가정.
	assert(CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(6)->GetName() == L"Item");

	if (_OtherObject->GetLayerIdx() != 6)
		return;

	// 주변부에서 제거함
	m_vecVicinity.erase(find(m_vecVicinity.begin(), m_vecVicinity.end(), _OtherObject));

	// UI에도 반영
	DisplayUI_Vicinity();
}
