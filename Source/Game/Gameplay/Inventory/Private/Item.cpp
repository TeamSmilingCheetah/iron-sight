#include "pch.h"
#include "Game/Gameplay/Inventory/Public/Item.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"

ItemScript::ItemScript()
	: CScript(ITEMSCRIPT)
	, m_ItemType(ITEM_TYPE::END)
	, m_Count(0)
{
}

ItemScript::ItemScript(ITEM_TYPE _Type)
	: CScript(ITEMSCRIPT)
	, m_ItemType(_Type)
	, m_Count(ItemMgr::GetInst()->GetItemInfo(m_ItemType).DefaultCount)
{
}

ItemScript::~ItemScript()
{
}

void ItemScript::Init()
{
}

void ItemScript::Begin()
{
	
}

void ItemScript::Tick()
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

void ItemScript::BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}

void ItemScript::Overlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}

void ItemScript::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}
