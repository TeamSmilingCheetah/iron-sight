#include "pch.h"
#include "Game/Gameplay/Inventory/Public/UI_Item.h"
#include "Game/Gameplay/Inventory/Public/Item.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"

#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"

ItemUI::ItemUI()
	: CScript(ITEMUI)
	, m_ItemObject(nullptr)
	, m_ItemInfo{}
	, m_Changed(false)
{
}

ItemUI::~ItemUI()
{
}

void ItemUI::SetItemObject(CGameObject* _Object)
{
	m_ItemObject = _Object;

	ItemScript* pScript = static_cast<ItemScript*>(m_ItemObject->GetScript(ITEMSCRIPT));
	assert(pScript);

	m_ItemInfo.Type = pScript->GetItemType();
	m_ItemInfo.Count = pScript->GetCount();

	m_Changed = true;
}

void ItemUI::SetItemInfo(ITEM_TYPE _Type, int _Count)
{
	m_ItemObject = nullptr;
	m_ItemInfo.Type = _Type;
	m_ItemInfo.Count = _Count;

	m_Changed = true;
}

void ItemUI::SyncItemUI()
{
	// ItemMgr에서 해당 Item 정보를 가져옴.
	const tItemCommonInfo info = ItemMgr::GetInst()->GetItemInfo(m_ItemInfo.Type);

	// UI 구조
	// base : 글씨(아이템 이름, 개수) 및 배경 색상 설정
	UI()->ClearText();
	UI()->AddText(info.Name, 45.f, 12.f, 12, FONT_RGBA(255, 255, 255, 255));
	wchar_t strCount[4]{};
	swprintf_s(strCount, L"%d", m_ItemInfo.Count);
	UI()->AddText(strCount, 130.f, 12.f, 12, FONT_RGBA(255, 255, 255, 255));

	// child : 배경 색상 및 아이템 이미지 설정
	GetOwner()->GetChild()[0]->UI()->SetImage(info.UIImage);
}

void ItemUI::Tick()
{
	if (m_Changed)
	{
		SyncItemUI();
		m_Changed = false;
	}
}

PayLoad ItemUI::OnMouseBeginDrag()
{
	PayLoad payload{};

	if (m_ItemObject)
	{
		payload.Type = L"ItemUI_Vicinity";
		payload.Data = reinterpret_cast<DWORD_PTR>(m_ItemObject);
	}
	else
	{
		payload.Type = L"ItemUI_Inventory";

		if (KEY_PRESSED(KEY::LCTRL))
		{
			m_ItemInfo.Count = 1;
		}

		payload.Data = reinterpret_cast<DWORD_PTR>(&m_ItemInfo);
	}

	return payload;
}
