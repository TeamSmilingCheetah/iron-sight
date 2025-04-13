#include "pch.h"
#include "Game/Gameplay/Inventory/Public/Item.h"

ItemScript::ItemScript()
	: CScript(ITEMSCRIPT)
	, m_Count(0)
	, m_ItemType(ITEM_TYPE::END)
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
}

void ItemScript::LoadComponent(FILE* _File)
{
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
