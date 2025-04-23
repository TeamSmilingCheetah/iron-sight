#include "pch.h"
#include "Game/Gameplay/Inventory/Public/UI_Vicinity.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"

VicinityUI::VicinityUI()
	: CScript(VICINITYUI)
{
}

VicinityUI::~VicinityUI()
{
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
