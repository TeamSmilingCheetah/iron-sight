#include "pch.h"
#include "Game/Gameplay/Inventory/Public/UI_Inventory.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"

InventoryUI::InventoryUI()
	: CScript(INVENTORYUI)
	, m_ControllerOwner(nullptr)
	, m_Controller(nullptr)
{
}

InventoryUI::~InventoryUI()
{
}

void InventoryUI::SetController(InventoryController* _Controller)
{
	m_Controller = _Controller;
	m_ControllerOwner = _Controller->GetOwner();
}

void InventoryUI::Begin()
{
}

void InventoryUI::SaveComponent(FILE* _File)
{
	SaveObjectRef(m_ControllerOwner, _File);
}

void InventoryUI::LoadComponent(FILE* _File)
{
	LoadObjectRef(m_ControllerOwner, _File);
}

void InventoryUI::LoadComponentReference()
{
	m_Controller = static_cast<InventoryController*>(m_ControllerOwner->GetScript(INVENTORYSCRIPT));
}

void InventoryUI::OnMouseDrop(const PayLoad& _PayLoad)
{
	// Vicinity에 있던 ItemUI를 Drop받은 경우
	if (_PayLoad.Type == L"ItemUI_Vicinity")
	{
		m_Controller->AcquireItem(reinterpret_cast<CGameObject*>(_PayLoad.Data));
	}
}
