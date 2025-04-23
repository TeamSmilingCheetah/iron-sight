#include "pch.h"
#include "Game/GamePlay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Inventory/Public/Item.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"
#include "Game/Gameplay/Inventory/Public/UI_Item.h"
#include "Game/Gameplay/Inventory/Public/UI_Vicinity.h"
#include "Game/Gameplay/Inventory/Public/UI_Inventory.h"

#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

InventoryController::InventoryController()
	: CScript(INVENTORYSCRIPT)
	, m_Player(nullptr)
	, m_PlayerScript(nullptr)
	, m_VicinityUI(nullptr)
	, m_InventoryUI(nullptr)
	, m_InventoryChanged(false)
	, m_arrInventory{}
{
}

InventoryController::~InventoryController()
{
}


void InventoryController::SetVicinityUI(CGameObject* _UI)
{
	m_VicinityUI = _UI;

	VicinityUI* pScript = static_cast<VicinityUI*>(_UI->GetScript(VICINITYUI));
	assert(pScript != nullptr);

	pScript->SetController(this);
}

void InventoryController::SetInventoryUI(CGameObject* _UI)
{
	m_InventoryUI = _UI;

	InventoryUI* pScript = static_cast<InventoryUI*>(_UI->GetScript(INVENTORYUI));
	assert(pScript != nullptr);

	pScript->SetController(this);
}

void InventoryController::SyncItemUI(CGameObject* _ItemObj, ITEM_TYPE _Type, int _Count, CGameObject* _ItemUI)
{
	ItemUI* pItemUI = reinterpret_cast<ItemUI*>(_ItemUI->GetScript(ITEMUI));
	assert(pItemUI != nullptr);

	// ItemUI 스크립트 쪽에 정보 전달
	if (_ItemObj != nullptr)
		pItemUI->SetItemObject(_ItemObj);
	else
		pItemUI->SetItemInfo(_Type, _Count);
}

void InventoryController::DisplayUI_Vicinity()
{
	const vector<CGameObject*>& vecVicinityUI = m_VicinityUI->GetChild();

	for (UINT i = 0; i < static_cast<UINT>(m_vecVicinity.size()); ++i)
	{
		ItemScript* pItem = static_cast<ItemScript*>(m_vecVicinity[i]->GetScript(ITEMSCRIPT));
		assert(pItem != nullptr);

		SyncItemUI(m_vecVicinity[i], pItem->GetItemType(), pItem->GetCount(), vecVicinityUI[i]);

		// UI 활성화
		vecVicinityUI[i]->SetActive(true);
	}

	// 대응되는 범위를 초과하는 UI는 비활성화
	for (UINT i = static_cast<UINT>(m_vecVicinity.size()); i < static_cast<UINT>(vecVicinityUI.size()); ++i)
	{
		vecVicinityUI[i]->SetActive(false);
	}
}

void InventoryController::DisplayUI_Inventory()
{
	const vector<CGameObject*>& vecInventoryUI = m_InventoryUI->GetChild();

	UINT uiIdx = 0;
	for (UINT i = 0; i < static_cast<UINT>(ITEM_TYPE::END); ++i)
	{
		if (m_arrInventory[i] == 0)
			continue;

		ITEM_TYPE type = static_cast<ITEM_TYPE>(i);
		int maxStackCount = ItemMgr::GetInst()->GetItemInfo(type).MaxCount;
		int curCount = m_arrInventory[i];

		// maxStackCount에 따라 ui를 나눔
		while (curCount > 0)
		{
			SyncItemUI(nullptr, type, min(curCount, maxStackCount), vecInventoryUI[uiIdx]);
			curCount -= maxStackCount;

			// UI의 위치 설정 (인덱스에 따라)
			vecInventoryUI[uiIdx]->UI()->SetRectPos(0.f, 200.f - 43.f * uiIdx);
			vecInventoryUI[uiIdx]->SetActive(true);

			++uiIdx;
		}
	}

	// 대응되는 범위를 초과하는 UI는 비활성화
	for (UINT i = uiIdx; i < static_cast<UINT>(vecInventoryUI.size()); ++i)
	{
		vecInventoryUI[i]->SetActive(false);
	}
}

void InventoryController::AcquireItem(CGameObject* _Item)
{
	ItemScript* pItem = static_cast<ItemScript*>(_Item->GetScript(ITEMSCRIPT));
	assert(pItem != nullptr);	// ItemScript가 있다는 가정

	UINT type = static_cast<UINT>(pItem->GetItemType());

	// 장착할 수 있는 무기인 경우 
	if(IS_WEAPON(type) || IS_THROWABLE(type))
	{
		// 오브젝트를 레이어에서 꺼냄
		ChangeLayer(_Item, -1);

		m_PlayerScript->EquipSlot(_Item);
	}
	else
	{
		// 오브젝트를 삭제
		DestroyObject(_Item);
	}

	// Inventory에 추가 (count)
	m_arrInventory[type] += pItem->GetCount();

	m_InventoryChanged = true;
}

void InventoryController::EquipItem(CGameObject* _Item)
{
	// WeaponScript가 있다는 가정
	assert(_Item->GetScript(WEAPONSCRIPT));


}


bool InventoryController::UseItem(ITEM_TYPE _Type, int _Count)
{
	// 일단 assert 걸었는데 로직에 따라 변경해야 할 수도
	assert(_Count >= m_arrInventory[static_cast<UINT>(_Type)]);

	m_arrInventory[static_cast<UINT>(_Type)] -= _Count;

	m_InventoryChanged = true;

	if (m_arrInventory[static_cast<UINT>(_Type)] == 0)
		return false;

	return true;
}

bool InventoryController::DropItem(ITEM_TYPE _Type, int _Count)
{
	// 일단 assert 걸었는데 로직에 따라 변경해야 할 수도
	assert(_Count >= m_arrInventory[static_cast<UINT>(_Type)]);
	
	UINT type = static_cast<UINT>(_Type);

	// 개수 조절
	m_arrInventory[type] -= _Count;

	// 장착 무기이면서 개수가 0개가 되면 슬롯에서 해제해줘야 함.
	if ((IS_WEAPON(type) || IS_THROWABLE(type)) && m_arrInventory[type] == 0)
	{
		m_PlayerScript->ReleaseSlot(_Type);
	}
	else
	{
		// TODO: Object Pooling으로 개선
		Ptr<CPrefab> itemPrefab = ItemMgr::GetInst()->GetItemInfo(_Type).Prefab;
		assert(itemPrefab != nullptr);

		CGameObject* itemObj = itemPrefab->Instantiate();
		itemObj->Transform()->SetRelativePos(Transform()->GetRelativePos());
		itemObj->Transform()->SetRelativeRotation(Transform()->GetRelativeRotation());

		// 오브젝트를 현재 레벨의 Item Layer에 추가함
		assert(CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(6)->GetName() == L"Item");
		CreateObject(itemObj, 6, true);
	}

	m_InventoryChanged = true;

	if (m_arrInventory[static_cast<UINT>(_Type)] == 0)
		return false;

	return true;
}

void InventoryController::Init()
{
}

void InventoryController::Begin()
{
	m_PlayerScript = static_cast<PlayerCharacter*>(m_Player->GetScript(PLAYERSCRIPT));
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
				AcquireItem(pItem);

				break;
			}
		}
	}

	if (m_InventoryChanged)
	{
		DisplayUI_Inventory();
		m_InventoryChanged = false;
	}
}

void InventoryController::SaveComponent(FILE* _File)
{
	SaveObjectRef(m_Player, _File);
	SaveObjectRef(m_VicinityUI, _File);
	SaveObjectRef(m_InventoryUI, _File);
}

void InventoryController::LoadComponent(FILE* _File)
{
	LoadObjectRef(m_Player, _File);
	LoadObjectRef(m_VicinityUI, _File);
	LoadObjectRef(m_InventoryUI, _File);
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
