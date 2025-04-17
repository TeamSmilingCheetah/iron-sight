#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"

class ItemUI
	: public CScript
{
private:
	CGameObject*	m_ItemObject;	// UI에 대응되는 아이템. null이면 inventory에 있는 itemui임

	tItemIndividualInfo		m_ItemInfo;

	bool	m_Changed;

public:
	void SetItemObject(CGameObject* _Object);
	void SetItemInfo(ITEM_TYPE _Type, int _Count);

private:
	void SyncItemUI();

public:
	void Tick() override;
	virtual void SaveComponent(FILE* _File) {}
	virtual void LoadComponent(FILE* _File) {}

	// UI Event
	virtual PayLoad OnMouseBeginDrag() override;

public:
	CLONE(ItemUI);
	ItemUI();
	~ItemUI();
};
