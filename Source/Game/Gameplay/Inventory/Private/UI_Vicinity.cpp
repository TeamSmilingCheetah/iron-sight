#include "Game/Gameplay/Inventory/Public/UI_Vicinity.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"
#include "Gameplay/Inventory/Public/ItemMgr.h"
#include "pch.h"

VicinityUI::VicinityUI()
	: CScript(SCRIPT_TYPE::VICINITYUI)
{
}

VicinityUI::~VicinityUI()
{
}

void VicinityUI::SetController(InventoryController* _Controller)
{
	m_Controller = _Controller;
	m_ControllerOwner = _Controller->GetOwner();
}

void VicinityUI::Begin()
{
}

void VicinityUI::SaveComponent(FILE* _File)
{
	SaveObjectRef(m_ControllerOwner, _File);
}

void VicinityUI::LoadComponent(FILE* _File)
{
	LoadObjectRef(m_ControllerOwner, _File);
}

void VicinityUI::LoadComponentReference()
{
	m_Controller = static_cast<InventoryController*>(GetScriptWithType(m_ControllerOwner, SCRIPT_TYPE::INVENTORYSCRIPT));
}

void VicinityUI::OnMouseDrop(const PayLoad& _PayLoad)
{
	// Inventory에 있던 ItemUI를 Drop받은 경우
	if (_PayLoad.Type == L"ItemUI_Inventory")
	{
		const tItemIndividualInfo& info = *reinterpret_cast<tItemIndividualInfo*>(_PayLoad.Data);
		m_Controller->DropItem(info.Type, info.Count);
	}
}
