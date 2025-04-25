#pragma once
#include "Engine\Runtime\Public\Component\Script\CScript.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"

class PlayerCharacter;
class ItemScript;

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

	// UI
	CGameObject*			m_VicinityUI;		// 주변 UI
	CGameObject*			m_InventoryUI;		// 인벤토리 UI

private:
	// 주변 : 오브젝트의 아이템 스크립트 -> UI
	void SyncItemUI(CGameObject* _ItemObj, ITEM_TYPE _Type, int _Count, CGameObject* _ItemUI);

	void DisplayUI_Vicinity();
	void DisplayUI_Inventory();

public:
	void SetPlayer(CGameObject* _Player) { m_Player = _Player; }
	void SetVicinityUI(CGameObject* _UI);
	void SetInventoryUI(CGameObject* _UI);

	void AcquireItem(CGameObject* _Item);
	void EquipItem(CGameObject* _Item);

	bool UseItem(ITEM_TYPE _Type, int _Count = 1);	// 아이템이 남으면 true 리턴
	bool DropItem(ITEM_TYPE _Type, int _Count);		// 아이템이 남으면 true 리턴

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

