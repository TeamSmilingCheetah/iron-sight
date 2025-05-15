#pragma once
#include "Engine\Runtime\Public\Component\Script\CScript.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"

class PlayerCharacter;
class ItemScript;

struct tSlot
{
	ITEM_TYPE		Type;
	CGameObject* Object;
};

#define PRIMARY_FIRST 0
#define PRIMARY_SECOND 1
#define SECONDARY_FIRST 2
#define THROWABLE_FIRST 3
#define THROWABLE_SECOND 4
#define NONE_WEAPON 5

class InventoryController :
	public CScript
{
private:
	CGameObject*			m_Player;
	PlayerCharacter*		m_PlayerScript;

	vector<CGameObject*>	m_vecVicinity;	// 주변 아이템을 관리하는 컨테이너
	int						m_arrInventory[static_cast<UINT>(ITEM_TYPE::END)];	// 소유 아이템을 관리하는 컨테이너 (개수)

	bool					m_VicinityChanged;
	bool					m_InventoryChanged;


	// 슬롯
	int			 m_CurSlotIdx;
	CGameObject* m_CurWeapon;

	vector<tSlot> m_vecWeaponSlot;	// 무기 슬롯

	class WeaponController* m_CurWeaponController;

	// bone object
	CGameObject* m_HandMeshObj;
	CGameObject* m_BackMeshObj;

	// UI
	CGameObject*			m_VicinityUI;		// 주변 UI
	CGameObject*			m_InventoryUI;		// 인벤토리 UI

private:
	// 주변 : 오브젝트의 아이템 스크립트 -> UI
	void SyncItemUI(CGameObject* _ItemObj, ITEM_TYPE _Type, int _Count, CGameObject* _ItemUI);

	void DisplayUI_Vicinity();
	void DisplayUI_Inventory();

public:
	void SetPlayer(CGameObject* _Player);
	void SetVicinityUI(CGameObject* _UI);
	void SetInventoryUI(CGameObject* _UI);

	void AcquireItem(CGameObject* _Item);
	void EquipItem(CGameObject* _Item);

	bool UseItem(ITEM_TYPE _Type, int _Count = 1);	// 아이템이 남으면 true 리턴
	void DropItem(ITEM_TYPE _Type, int _Count);		// 아이템이 남으면 true 리턴

	void PlayerInteractWeapon();

	void EquipWeapon(CGameObject* _Item);

	// TODO: 데이터 구조 개선
	void AttachItem(CGameObject* _Item, CGameObject* _BoneObject, Vec3 _RelativePos, Vec3 _RelativeRot);
	void DetachItem(CGameObject* _Item);

	// 현재 슬롯에 새로운 아이템을 장착하는 함수. 기존에 있던 아이템을 리턴해줌 -> 알아서 처리
	CGameObject* EquipSlot(CGameObject* _Item, ITEM_TYPE _Type, int _SlotIdx, bool _ChangeSlotIdx = true);

	// Type이 일치하는 슬롯을 해제하고 해당 오브젝트를 리턴함.
	CGameObject* ReleaseSlot(ITEM_TYPE _Type);

	// 현재 슬롯을 변경하는 함수. 무기 집어넣고 꺼내는 애니메이션 trigger 추가하기
	void ChangeSlot(int _SlotIdx);

	// Slot 값을 초기화 하는 함수
	void ClearSlot(int _SlotIdx);

	void ActivateSlot(int _SlotIdx); // 현재 슬롯을 지정한 슬롯으로 변경
	void DeactivateSlot();	// 현재 슬롯을 비활성화

	void AddItemToVicinity(CGameObject* _Item);
	void RemoveItemFromVicinity(CGameObject* _Item);

public:
	int GetCurSlotIdx() const { return m_CurSlotIdx; }
	CGameObject* GetCurWeapon() const { return m_CurWeapon; }
	WeaponController* GetCurWeaponController() const { return m_CurWeaponController; }

public:
	virtual void Init() override;
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) override;
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) override;
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) override;


public:
	CLONE(InventoryController);
	InventoryController();
	~InventoryController();
};
