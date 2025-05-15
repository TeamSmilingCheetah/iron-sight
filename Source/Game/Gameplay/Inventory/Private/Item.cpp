#include "pch.h"
#include "Game/Gameplay/Inventory/Public/Item.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"
#include "Game/GamePlay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Character/Public/InteractionHandler.h"

ItemScript::ItemScript()
	: InteractableScript(ITEMSCRIPT)
	, m_ItemType(ITEM_TYPE::END)
	, m_Count(0)
{
}

ItemScript::ItemScript(ITEM_TYPE _Type)
	: InteractableScript(ITEMSCRIPT)
	, m_ItemType(_Type)
	, m_Count(ItemMgr::GetInst()->GetItemInfo(m_ItemType).DefaultCount)
{
}

ItemScript::~ItemScript()
{
}

void ItemScript::SaveComponent(FILE* _File)
{
	fwrite(&m_ItemType, sizeof(ITEM_TYPE), 1, _File);
}

void ItemScript::LoadComponent(FILE* _File)
{
	fread(&m_ItemType, sizeof(ITEM_TYPE), 1, _File);
	m_Count = ItemMgr::GetInst()->GetItemInfo(m_ItemType).DefaultCount;
}

void ItemScript::EnterDetection(InteractionHandler* _Handler)
{
	InventoryController* pInventory = static_cast<InventoryController*>(_Handler->GetOwner()->GetParent()->GetScript(INVENTORYSCRIPT));

	assert(pInventory);

	pInventory->AddItemToVicinity(GetOwner());
}

void ItemScript::Interact(InteractionHandler* _Handler)
{
	InventoryController* pInventory = static_cast<InventoryController*>(_Handler->GetOwner()->GetParent()->GetScript(INVENTORYSCRIPT));

	assert(pInventory);

	pInventory->AcquireItem(GetOwner());
}

void ItemScript::ExitDetection(InteractionHandler* _Handler)
{
	InventoryController* pInventory = static_cast<InventoryController*>(_Handler->GetOwner()->GetParent()->GetScript(INVENTORYSCRIPT));

	assert(pInventory);

	pInventory->RemoveItemFromVicinity(GetOwner());
}
