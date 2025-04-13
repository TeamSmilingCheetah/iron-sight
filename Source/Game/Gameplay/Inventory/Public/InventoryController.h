#pragma once
#include "Engine\Runtime\Public\Component\Script\CScript.h"

class PlayerCharacter;
class ItemScript;

class InventoryController :
    public CScript
{
private:
	CGameObject*		m_Player;
	PlayerCharacter*	m_PlayerScript;

	vector<CGameObject*>	m_vecVicinity;	// 주변 아이템을 관리하는 컨테이너
	vector<CGameObject*>	m_vecInventory;	// 소유 아이템을 관리하는 컨테이너

	vector<CGameObject*>	m_vecVicinityUI;	// 주변 아이템 UI
	vector<CGameObject*>	m_vecInventoryUI;	// 소유 아이템 UI

private:
	void SyncItemUI(ItemScript* _Item, CGameObject* _ItemUI);
	void DisplayUI_Vicinity();
	void DisplayUI_Inventory();

public:
	void SetPlayer(CGameObject* _Player);
	void AddVicinityUI(CGameObject* _UI) { m_vecVicinityUI.push_back(_UI); }
	void AddInventoryUI(CGameObject* _UI) { m_vecInventoryUI.push_back(_UI); }

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

