#include "pch.h"
#include "Game/GamePlay/Inventory/Public/InventoryController.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Inventory/Public/Item.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"
#include "Game/Gameplay/Inventory/Public/UI_Item.h"
#include "Game/Gameplay/Inventory/Public/UI_Vicinity.h"
#include "Game/Gameplay/Inventory/Public/UI_Inventory.h"
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Character/Public/CameraController.h"

InventoryController::InventoryController()
	: CScript(SCRIPT_TYPE::INVENTORYSCRIPT)
	, m_Player(nullptr)
	, m_PlayerScript(nullptr)
	, m_VicinityUI(nullptr)
	, m_InventoryUI(nullptr)
	, m_InventoryChanged(false)
	, m_arrInventory{}
	, m_CurSlotIdx(NONE_WEAPON)
	, m_HandMeshObj(nullptr)
	, m_BackMeshObj(nullptr)
{
	m_vecWeaponSlot.resize(5);
	char name[10]{};
	for (int i = 0; i < static_cast<int>(m_vecWeaponSlot.size()); ++i)
	{
		sprintf_s(name, "Slot %d", i + 1);
		AddScriptParam(tScriptParam{ SCRIPT_PARAM::GAMEOBJECT, name, &m_vecWeaponSlot[i].Object });
	}
}

InventoryController::~InventoryController()
{
}

void InventoryController::Init()
{
}

void InventoryController::Begin()
{
	// Bone
	vector<CGameObject*> vecBones = m_Player->Animator3D()->GetvecBone();

	for (int i = 0; i < vecBones.size(); ++i)
	{
		if (vecBones[i]->GetName() == L"mixamorig:RightHand")
		{
			m_HandMeshObj = vecBones[i];
		}

		if (vecBones[i]->GetName() == L"mixamorig:RightShoulder")
		{
			m_BackMeshObj = vecBones[i];
		}
	}


	m_CamScript = static_cast<CameraController*>(GetScriptWithType(CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera"), SCRIPT_TYPE::CAMERASCRIPT));
}

void InventoryController::Tick()
{
	if (m_VicinityChanged)
	{
		DisplayUI_Vicinity();
		m_VicinityChanged = false;
	}

	if (m_InventoryChanged)
	{
		DisplayUI_Inventory();
		m_InventoryChanged = false;
	}

	PlayerInteractWeapon();
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

void InventoryController::LoadComponentReference()
{
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));
}

void InventoryController::SetPlayer(CGameObject* _Player)
{
	m_Player = _Player;

	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));
}

void InventoryController::SetVicinityUI(CGameObject* _UI)
{
	m_VicinityUI = _UI;

	VicinityUI* pScript = static_cast<VicinityUI*>(GetScriptWithType(_UI, SCRIPT_TYPE::VICINITYUI));
	assert(pScript != nullptr);

	pScript->SetController(this);
}

void InventoryController::SetInventoryUI(CGameObject* _UI)
{
	m_InventoryUI = _UI;

	InventoryUI* pScript = static_cast<InventoryUI*>(GetScriptWithType(_UI, SCRIPT_TYPE::INVENTORYUI));
	assert(pScript != nullptr);

	pScript->SetController(this);
}

void InventoryController::SyncItemUI(CGameObject* _ItemObj, ITEM_TYPE _Type, int _Count, CGameObject* _ItemUI)
{
	ItemUI* pItemUI = reinterpret_cast<ItemUI*>(GetScriptWithType(_ItemUI, SCRIPT_TYPE::ITEMUI));
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
		ItemScript* pItem = static_cast<ItemScript*>(GetScriptWithType(m_vecVicinity[i], SCRIPT_TYPE::ITEMSCRIPT));
		assert(pItem != nullptr);

		SyncItemUI(m_vecVicinity[i], pItem->GetItemType(), pItem->GetCount(), vecVicinityUI[i]);

		// UI 활성화
		SetObjectActive(vecVicinityUI[i], true);
	}

	// 대응되는 범위를 초과하는 UI는 비활성화
	for (UINT i = static_cast<UINT>(m_vecVicinity.size()); i < static_cast<UINT>(vecVicinityUI.size()); ++i)
	{
		SetObjectActive(vecVicinityUI[i], false);
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
			SetObjectActive(vecInventoryUI[uiIdx], true);

			++uiIdx;
		}
	}

	// 대응되는 범위를 초과하는 UI는 비활성화
	for (UINT i = uiIdx; i < static_cast<UINT>(vecInventoryUI.size()); ++i)
	{
		SetObjectActive(vecInventoryUI[i], false);
	}
}

void InventoryController::AcquireItem(CGameObject* _Item)
{
	ItemScript* pItem = static_cast<ItemScript*>(GetScriptWithType(_Item, SCRIPT_TYPE::ITEMSCRIPT));
	assert(pItem != nullptr);	// ItemScript가 있다는 가정

	UINT type = static_cast<UINT>(pItem->GetItemType());

	// 장착할 수 있는 무기인 경우
	if(IS_WEAPON(type) || IS_THROWABLE(type))
	{
		EquipWeapon(_Item);
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
	assert(GetScriptWithType(_Item, SCRIPT_TYPE::WEAPONSCRIPT));


}

void InventoryController::ConvertPS()
{
	// 착용중인 무기가 없으면 수행 X
	if (m_CurWeapon == nullptr)
		return;

	// 현재 TPS이다 (즉, FPS->TPS로 변환 됨)
	if (m_CamScript->GetFlag(TPS))
	{
		// 현재 착용중인 무기를 다시 Bone에 붙혀준다.
		AttachItem(m_CurTempWeapon, m_HandMeshObj, Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
	}

	// 현재 FPS이다. (즉, TPS->FPS로 변환 됨)
	else
	{
		m_CurTempWeapon = m_CurWeapon;
		DetachItem(m_CurWeapon, false, 4);
	}

}


bool InventoryController::UseItem(ITEM_TYPE _Type, int _Count)
{
	// 일단 assert 걸었는데 로직에 따라 변경해야 할 수도
	assert(_Count <= m_arrInventory[static_cast<UINT>(_Type)]);

	m_arrInventory[static_cast<UINT>(_Type)] -= _Count;

	m_InventoryChanged = true;

	if (m_arrInventory[static_cast<UINT>(_Type)] == 0)
		return false;

	return true;
}

void InventoryController::DropItem(ITEM_TYPE _Type, int _Count)
{
	// 일단 assert 걸었는데 로직에 따라 변경해야 할 수도
	assert(_Count <= m_arrInventory[static_cast<UINT>(_Type)]);

	UINT type = static_cast<UINT>(_Type);

	// 개수 조절
	m_arrInventory[type] -= _Count;

	// 떨어트릴 오브젝트
	CGameObject* DropItem = nullptr;

	assert(CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(6)->GetName() == L"Item");

	// 장착 무기이면서 개수가 0개가 되면 슬롯에서 해제해줘야 함.
	if ((IS_WEAPON(type) || IS_THROWABLE(type)) && m_arrInventory[type] == 0)
	{
		DropItem = ReleaseSlot(_Type);
	}
	else
	{
		// TODO: Object Pooling으로 개선
		Ptr<CPrefab> itemPrefab = ItemMgr::GetInst()->GetItemInfo(_Type).Prefab;
		assert(itemPrefab != nullptr);

		DropItem = itemPrefab->Instantiate();

		// 오브젝트를 현재 레벨의 Item Layer에 추가함
		CreateObject(DropItem, 6, true);
	}

	// 개수 설정
	static_cast<ItemScript*>(GetScriptWithType(DropItem, SCRIPT_TYPE::ITEMSCRIPT))->SetCount(_Count);

	// 부모 관계 해제, 레이어 이동, transform 설정
	DetachItem(DropItem);

	m_InventoryChanged = true;
}

// Compare
bool ItemComp(CGameObject* _lhs, CGameObject* _rhs)
{
	ItemScript* lItem = static_cast<ItemScript*>(GetScriptWithType(_lhs, SCRIPT_TYPE::ITEMSCRIPT));
	ItemScript* rItem = static_cast<ItemScript*>(GetScriptWithType(_rhs, SCRIPT_TYPE::ITEMSCRIPT));

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
}

void InventoryController::Overlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}

void InventoryController::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}

void InventoryController::EquipWeapon(CGameObject* _Item)
{
	// 장착할 수 있는 아이템이라는 것이 보장되어 있음
	auto pWeaponScript = static_cast<WeaponController*>(GetScriptWithType(_Item, SCRIPT_TYPE::WEAPONSCRIPT));

	assert(pWeaponScript != nullptr);		// WeaponScript가 있다는 가정

	WEAPON_TYPE eWeaponType = pWeaponScript->GetWeaponType();
	static bool bCanEquip = false;

	switch (eWeaponType)
	{
		// 주무기
	case WEAPON_TYPE::PRIMARY:
	{
		int slotIdx = 0;
		for (int i = PRIMARY_FIRST; i <= PRIMARY_SECOND; ++i)
		{
			if (m_vecWeaponSlot[i].Object == nullptr)
			{
				slotIdx = i;
				break;
			}
		}

		// 빈 슬롯이 없었다면 첫번째 슬롯과 교체
		if (m_vecWeaponSlot[slotIdx].Object)
		{
			// 현재 슬롯을 해제
			//DetachItem(m_vecWeaponSlot[slotIdx].Object);
			//m_InventoryScript->UseItem(m_vecWeaponSlot[slotIdx].Type);
			DropItem(m_vecWeaponSlot[slotIdx].Type, 1);
		}

		m_vecWeaponSlot[slotIdx].Object = _Item;
		m_vecWeaponSlot[slotIdx].Type = static_cast<ItemScript*>(GetScriptWithType(_Item, SCRIPT_TYPE::ITEMSCRIPT))->GetItemType();

		bCanEquip = true;

		if (slotIdx == PRIMARY_FIRST)
		{
			AttachItem(_Item, m_BackMeshObj, Vec3(-810.f, -99.f, -234.f), Vec3(75.9f, -2.f, -4.3f));
		}
		else
		{
			AttachItem(_Item, m_BackMeshObj, Vec3(-810.f, -99.f, 75.f), Vec3(75.9f, -2.f, -4.3f));
		}

		SetObjectActive(_Item, true);
	}
	break;
	// 보조무기
	case WEAPON_TYPE::SECONDARY:
		if (m_vecWeaponSlot[SECONDARY_FIRST].Object == nullptr)
		{
			m_vecWeaponSlot[SECONDARY_FIRST].Object = _Item;
			m_vecWeaponSlot[SECONDARY_FIRST].Type = static_cast<ItemScript*>(GetScriptWithType(_Item, SCRIPT_TYPE::ITEMSCRIPT))->GetItemType();
			AddChild(m_Player, _Item);
			SetObjectActive(_Item, false);
			bCanEquip = true;
		}
		break;
		// 투척무기
	case WEAPON_TYPE::THROWABLE:
	{
		for (int i = THROWABLE_FIRST; i <= THROWABLE_SECOND; ++i)
		{
			// 슬롯에 이미 차 있는 경우
			if (m_vecWeaponSlot[i].Object != nullptr)
			{
				// 같은 weapon 인 경우
				if (m_vecWeaponSlot[i].Type == static_cast<ItemScript*>(GetScriptWithType(_Item, SCRIPT_TYPE::ITEMSCRIPT))->GetItemType())
					break;

				// 다른 weapon인 경우
				continue;
			}

			m_vecWeaponSlot[i].Object = _Item;
			m_vecWeaponSlot[i].Type = static_cast<ItemScript*>(GetScriptWithType(_Item, SCRIPT_TYPE::ITEMSCRIPT))->GetItemType();
			AttachItem(_Item, m_HandMeshObj, Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
			SetObjectActive(_Item, false);
			bCanEquip = true;
			break;
		}

	}
	break;
	default:
		break;
	}

	// 장착할 수 있는 경우 :
	// 무기 스크립트 쪽에 알려줘야 함
	if (bCanEquip)
	{
		// weaponcontroller 등록
		m_CurWeaponController = pWeaponScript;

		// Player를 소유주으로 등록, 자식에 무기를 넣어준다.
		pWeaponScript->SetEquippedOwner(m_Player);

		bCanEquip = false;
	}

	// 장착할 수 없는 경우 :
	// 이미 해당 타입이 슬롯에 존재하거나 빈 슬롯이 없었다면 아이템 정보만 얻고 객체는 파괴함
	else
	{
		// TODO : object pooling으로 개선 (파괴가 아니라 비활성화 후 pool에 넣기)
		DestroyObject(_Item);
	}
}

CGameObject* InventoryController::EquipSlot(CGameObject* _Item, ITEM_TYPE _Type, int _SlotIdx, bool _ChangeSlotIdx)
{
	// 슬롯에 있던 기존 아이템 오브젝트 받아둠
	CGameObject* pPrevObj = m_vecWeaponSlot[_SlotIdx].Object;

	int curSlotIdx = m_CurSlotIdx;

	ITEM_TYPE type = static_cast<ItemScript*>(GetScriptWithType(pPrevObj, SCRIPT_TYPE::ITEMSCRIPT))->GetItemType();

	// 기존 슬롯 아이템에 대한 두 가지 선택지
	if (IS_WEAPON(static_cast<UINT>(type)))
	{
		// 1. 바닥에 떨어트린다 (총)
		DropItem(type, 1);
	}
	else // if (IS_THROWABLE(type))
	{
		// 2. 그냥 slot에서만 해제한다 (투척무기)
		ClearSlot(_SlotIdx);
	}

	// 지정한 슬롯에 무기 설정
	m_vecWeaponSlot[_SlotIdx].Object = _Item;
	m_vecWeaponSlot[_SlotIdx].Type = _Type;

	auto pWeaponScript = static_cast<WeaponController*>(GetScriptWithType(_Item, SCRIPT_TYPE::WEAPONSCRIPT));

	pWeaponScript->SetEquippedOwner(m_Player);
	pWeaponScript->SetEquip(true);

	// 현재 슬롯과 동일하다면
	if (m_CurSlotIdx == NONE_WEAPON)
	{
		m_CurWeapon = _Item;
		m_CurSlotIdx = _SlotIdx;
		m_CurWeaponController = pWeaponScript;
	}


	// 장착한 슬롯을 현재 슬롯으로 바꿔준다.
	if (_ChangeSlotIdx && _SlotIdx != m_CurSlotIdx)
	{
		ChangeSlot(_SlotIdx);
	}

	return pPrevObj;
}

CGameObject* InventoryController::ReleaseSlot(ITEM_TYPE _Type)
{
	CGameObject* prevObj = nullptr;

	for (int i = 0; i < static_cast<int>(m_vecWeaponSlot.size()); ++i)
	{
		if (m_vecWeaponSlot[i].Type == _Type)
		{
			prevObj = m_vecWeaponSlot[i].Object;

			// clear
			ClearSlot(i);

			return prevObj;
		}
	}

	return nullptr;
}



void InventoryController::ChangeSlot(int _SlotIdx)
{
	DeactivateSlot();

	ActivateSlot(_SlotIdx);

	if (_SlotIdx == m_CurSlotIdx)
		return;

	// TODO : Animation Trigger 등

}

void InventoryController::ChangeCurTemp(int _SlotIdx)
{
	if (_SlotIdx == m_CurSlotIdx)
		return;


	DeactivateSlot(!m_CamScript->GetFlag(TPS));


	WeaponController* pWeaponScript = static_cast<WeaponController*>(GetScriptWithType(m_vecWeaponSlot[_SlotIdx].Object, SCRIPT_TYPE::GUNSCRIPT));
	if (pWeaponScript == nullptr)
		pWeaponScript = static_cast<WeaponController*>(GetScriptWithType(m_vecWeaponSlot[_SlotIdx].Object, SCRIPT_TYPE::THROWABLESCRIPT));

	// weaponController 장착 전달
	pWeaponScript->SetEquip(true);

	// 현재 슬롯으로 변경
	m_CurWeapon = m_vecWeaponSlot[_SlotIdx].Object;
	m_CurWeaponController = pWeaponScript;
	m_CurSlotIdx = _SlotIdx;
	ConvertPS();
}


void InventoryController::ActivateSlot(int _SlotIdx)
{
	auto pWeaponScript = static_cast<WeaponController*>(GetScriptWithType(m_vecWeaponSlot[_SlotIdx].Object, SCRIPT_TYPE::WEAPONSCRIPT));

	// weaponController 장착 전달
	pWeaponScript->SetEquip(true);

	// 현재 슬롯으로 변경
	m_CurWeapon = m_vecWeaponSlot[_SlotIdx].Object;
	m_CurWeaponController = pWeaponScript;
	m_CurSlotIdx = _SlotIdx;

	// 활성화 로직 -> type에 따라 다름
	if (_SlotIdx == PRIMARY_FIRST || _SlotIdx == PRIMARY_SECOND)
	{
		AttachItem(m_CurWeapon, m_HandMeshObj, Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
	}
	else
	{
		// 슬롯 오브젝트 활성화
		SetObjectActive(m_vecWeaponSlot[_SlotIdx].Object, true);
	}
}

void InventoryController::DeactivateSlot(bool _FPS)
{
	// 현재 슬롯이 없다면
	if (m_CurSlotIdx == NONE_WEAPON)
		return;

	// weaponController 장착 해제 전달
	m_CurWeaponController->SetEquip(false);

	// 비활성화 로직
	// 주무기는 비활성화가 아닌 등에 다시 부착 시켜 준다.
	if (m_CurSlotIdx == PRIMARY_FIRST)
	{
		AttachItem(m_vecWeaponSlot[m_CurSlotIdx].Object, m_BackMeshObj, Vec3(-810.f, -99.f, -234.f), Vec3(75.9f, -2.f, -4.3f));
	}
	else if (m_CurSlotIdx == PRIMARY_SECOND)
	{
		AttachItem(m_vecWeaponSlot[m_CurSlotIdx].Object, m_BackMeshObj, Vec3(-810.f, -99.f, 75.f), Vec3(75.9f, -2.f, -4.3f));
	}
	// 나머지는 비활성화 해준다.
	// 1인칭인경우 Player HandMesh에 다시 붙혀준다.
	else if (_FPS)
	{
		AttachItem(m_vecWeaponSlot[m_CurSlotIdx].Object, m_HandMeshObj, Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
		SetObjectActive(m_vecWeaponSlot[m_CurSlotIdx].Object, false);
	}
	else
	{
		SetObjectActive(m_vecWeaponSlot[m_CurSlotIdx].Object, false);
	}

	// 현재 슬롯 관련 정보 초기화
	m_CurWeapon = nullptr;
	m_CurWeaponController = nullptr;
	m_CurSlotIdx = NONE_WEAPON;
}

void InventoryController::AddItemToVicinity(CGameObject* _Item)
{
	// 주변부에 추가함
	vector<CGameObject*> vecTemp;

	// 하나를 삽입하는 거니까 sort보다 O(N)의 방법 채택
	bool isInserted = false;

	for (size_t i = 0; i < m_vecVicinity.size(); ++i)
	{
		if (!isInserted && !ItemComp(m_vecVicinity[i], _Item))
		{
			vecTemp.push_back(_Item);

			isInserted = true;
		}

		vecTemp.push_back(m_vecVicinity[i]);
	}

	if (!isInserted)
		vecTemp.push_back(_Item);

	m_vecVicinity = std::move(vecTemp);

	// UI에도 반영
	m_VicinityChanged = true;
}

void InventoryController::RemoveItemFromVicinity(CGameObject* _Item)
{
	// 주변부에서 제거함
	m_vecVicinity.erase(find(m_vecVicinity.begin(), m_vecVicinity.end(), _Item));

	// UI에도 반영
	m_VicinityChanged = true;
}

void InventoryController::AttachItem(CGameObject* _Item, CGameObject* _BoneObject, Vec3 _RelativePos, Vec3 _RelativeRot)
{
	AddChild(_BoneObject, _Item);
	_Item->Transform()->SetRelativePos(_RelativePos);
	_Item->Transform()->SetRelativeRotation(_RelativeRot);
}

void InventoryController::DetachItem(CGameObject* _Item, bool _Disconnect, int _Layer)
{
	Vec3 vPos = m_Player->Transform()->GetRelativePos();
	Vec3 vRot = m_Player->Transform()->GetRelativeRotation();

	// 주인 해제
	auto pWeaponScript = static_cast<WeaponController*>(GetScriptWithType(_Item, SCRIPT_TYPE::WEAPONSCRIPT));

	if (pWeaponScript && _Disconnect)
	{
		pWeaponScript->SetEquippedOwner(nullptr);
		pWeaponScript->SetEquip(false);
	}

	// 아이템 레이어로 변경
	if(_Layer == 6)
		assert(CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(_Layer)->GetName() == L"Item");
	else if (_Layer == 4)
		assert(CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(_Layer)->GetName() == L"PlayerFPS");
	ChangeLayer(_Item, _Layer);
	SetObjectActive(_Item, true);
	AttachItem(_Item, nullptr, vPos, vRot);
}

void InventoryController::ClearSlot(int _SlotIdx)
{
	m_vecWeaponSlot[_SlotIdx].Object = nullptr;
	m_vecWeaponSlot[_SlotIdx].Type = ITEM_TYPE::END;

	// 현재 슬롯이라면
	if (_SlotIdx == m_CurSlotIdx)
	{
		m_CurWeapon = nullptr;
		m_CurSlotIdx = NONE_WEAPON;
		m_CurWeaponController = nullptr;
	}
}


void InventoryController::PlayerInteractWeapon()
{
	// 무기 교체
	for (int i = 0; i < static_cast<int>(m_vecWeaponSlot.size()); ++i)
	{
		if (!m_vecWeaponSlot[i].Object)
			continue;

		if (KEY_TAP(static_cast<KEY>(static_cast<int>(KEY::NUM_1) + i)))
		{
			// 장전중이라면 취소시킨다.
			if (m_PlayerScript->IsPlayerReloading())
			{
				m_PlayerScript->SetReloading(false);
				m_PlayerScript->SetReloadingEnd(true);
			}

			if (!m_CamScript->GetFlag(TPS))
			{
				// 현재 FPS모드라면 비활성화할 무기 대상을 바꿔준다.
				ChangeCurTemp(i);
			}
			else
			{
				// 해당 슬롯 활성화
				ChangeSlot(i);
			}

			if (m_CamScript->GetFlag(WAS_TPS))
			{
				m_CamScript->SetFlag(ADS, false);
				m_CamScript->SetFlag(WAS_TPS, false);
				m_CamScript->ChangePS(true);
			}
			else
			{
				m_CamScript->SetFlag(ADS, false);
			}
			m_CamScript->SetFlag(SHOULDER, false);

			m_bWeaponChange = true;
			break;
		}
	}

	// 모든 무기 내리기 (무기 미착용 상태로)
	if (KEY_TAP(KEY::X))
	{
		DeactivateSlot(!m_CamScript->GetFlag(TPS));

		// 장전중이라면 취소시킨다.
		if (m_PlayerScript->IsPlayerReloading())
		{
			m_PlayerScript->SetReloading(false);
			m_PlayerScript->SetReloadingEnd(true);
		}

		m_CurWeapon = nullptr;
		m_CurSlotIdx = NONE_WEAPON;
		m_CurWeaponController = nullptr;
		m_bWeaponChange = true;
	}

	// 현재 들고 있는 무기를 버린다.
	if (KEY_TAP(KEY::G))
	{
		if (m_CurSlotIdx != NONE_WEAPON)
		{
			// 인벤토리에서 아이템 개수 관리
			DropItem(m_vecWeaponSlot[m_CurSlotIdx].Type, 1);
			m_bWeaponChange = true;
		}
	}

}
