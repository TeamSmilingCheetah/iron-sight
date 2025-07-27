#pragma once
#include "Game/Gameplay/Interaction/Public/InteractableScript.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"

class ItemScript
	: public InteractableScript
{
private:
	ITEM_TYPE	m_ItemType;
	int			m_Count;

public:
	ITEM_TYPE GetItemType() const { return m_ItemType; }
	void SetItemType(ITEM_TYPE _Type) { m_ItemType = _Type; }

	int GetCount() const { return m_Count; }
	void SetCount(UINT _Count) { m_Count = _Count; }

public:
	virtual void Init() override {}
	virtual void Begin() override {}
	virtual void Tick() override {}

	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

	void BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override {}
	void Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override {}
	void EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override {}

	virtual void EnterDetection(InteractionHandler* _Handler) override;
	virtual void Interact(InteractionHandler* _Handler) override;
	virtual void ExitDetection(InteractionHandler* _Handler) override;

public:
	SET_PARENT_SCRIPT(SCRIPT_TYPE::INTERACTABLE);

public:
	CLONE(ItemScript);
	ItemScript();
	ItemScript(ITEM_TYPE _Type);
	~ItemScript();
};
