#include "pch.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"

ItemMgr::ItemMgr()
	: m_ItemInfo{}
{
}

ItemMgr::~ItemMgr()
{
}

void ItemMgr::Init()
{
	// 일단 스크립트에 구현
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AKM)].Name = L"AKM";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AKM)].DefaultCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AKM)].MaxCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AKM)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AKM)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\AKM.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AKM)].Prefab = FAssetManager::GetInst()->Load<CPrefab>(L"Prefab\\AKM.pref");

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FIRST_AID_KIT)].Name = L"구급상자";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FIRST_AID_KIT)].DefaultCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FIRST_AID_KIT)].MaxCount = 10;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FIRST_AID_KIT)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FIRST_AID_KIT)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\FirstAidKit.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FIRST_AID_KIT)].Prefab = FAssetManager::GetInst()->Load<CPrefab>(L"Prefab\\FirstAidKit.pref");

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MED_KIT)].Name = L"의료용 키트";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MED_KIT)].DefaultCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MED_KIT)].MaxCount = 10;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MED_KIT)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MED_KIT)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\MedKit.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MED_KIT)].Prefab = nullptr;

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::BANDAGE)].Name = L"붕대";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::BANDAGE)].DefaultCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::BANDAGE)].MaxCount = 20;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::BANDAGE)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::BANDAGE)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\Bandage.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::BANDAGE)].Prefab = nullptr;

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ADRENALINE_SYRINGE)].Name = L"아드레날린";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ADRENALINE_SYRINGE)].DefaultCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ADRENALINE_SYRINGE)].MaxCount = 5;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ADRENALINE_SYRINGE)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ADRENALINE_SYRINGE)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\AdrenalineSyringe.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ADRENALINE_SYRINGE)].Prefab = FAssetManager::GetInst()->Load<CPrefab>(L"Prefab\\Adrenaline.pref");

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::PAIN_KILLER)].Name = L"진통제";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::PAIN_KILLER)].DefaultCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::PAIN_KILLER)].MaxCount = 10;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::PAIN_KILLER)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::PAIN_KILLER)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\PainKiller.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::PAIN_KILLER)].Prefab = nullptr;

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ENERGY_DRINK)].Name = L"에너지드링크";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ENERGY_DRINK)].DefaultCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ENERGY_DRINK)].MaxCount = 10;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ENERGY_DRINK)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ENERGY_DRINK)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\EnergyDrink.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::ENERGY_DRINK)].Prefab = FAssetManager::GetInst()->Load<CPrefab>(L"Prefab\\EnergyDrink.pref");

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::GRENADE)].Name = L"수류탄";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::GRENADE)].DefaultCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::GRENADE)].MaxCount = 5;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::GRENADE)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::GRENADE)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\Grenade.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::GRENADE)].Prefab = FAssetManager::GetInst()->Load<CPrefab>(L"Prefab\\Grenade.pref");

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MOLOTOV)].Name = L"화염병";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MOLOTOV)].DefaultCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MOLOTOV)].MaxCount = 5;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MOLOTOV)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MOLOTOV)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\Molotov.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::MOLOTOV)].Prefab = nullptr;

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::SMOKEBOMB)].Name = L"연막탄";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::SMOKEBOMB)].DefaultCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::SMOKEBOMB)].MaxCount = 5;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::SMOKEBOMB)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::SMOKEBOMB)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\SmokeBomb.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::SMOKEBOMB)].Prefab = FAssetManager::GetInst()->Load<CPrefab>(L"Prefab\\SmokeBomb.pref");

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FLASHBOMB)].Name = L"섬광탄";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FLASHBOMB)].DefaultCount = 1;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FLASHBOMB)].MaxCount = 5;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FLASHBOMB)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FLASHBOMB)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\FlashBang.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::FLASHBOMB)].Prefab = nullptr;

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_9)].Name = L"9mm";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_9)].DefaultCount = 30;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_9)].MaxCount = 200;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_9)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_9)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\Ammo_9.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_9)].Prefab = nullptr;

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_5)].Name = L"5.56mm";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_5)].DefaultCount = 30;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_5)].MaxCount = 200;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_5)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_5)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\Ammo_5.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_5)].Prefab = FAssetManager::GetInst()->Load<CPrefab>(L"Prefab\\TestAmmo.pref");

	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_7)].Name = L"7.62mm";
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_7)].DefaultCount = 30;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_7)].MaxCount = 200;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_7)].WeightPerCount = 0.f;
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_7)].UIImage = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\Ammo_7.png");
	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::AMMO_7)].Prefab = nullptr;
}

//void ItemMgr::SaveComponent(FILE* _File)
//{
//	for (UINT i = 0; i < static_cast<UINT>(ITEM_TYPE::END); ++i)
//	{
//		SaveWString(m_ItemInfo[i].Name, _File);
//		fwrite(&m_ItemInfo[i].DefaultCount, sizeof(UINT), 1, _File);
//		fwrite(&m_ItemInfo[i].WeightPerCount, sizeof(float), 1, _File);
//		SaveAssetRef(m_ItemInfo[i].UIImage, _File);
//		SaveAssetRef(m_ItemInfo[i].Prefab, _File);
//	}
//}
//
//void ItemMgr::LoadComponent(FILE* _File)
//{
//	for (UINT i = 0; i < static_cast<UINT>(ITEM_TYPE::END); ++i)
//	{
//		LoadWString(m_ItemInfo[i].Name, _File);
//		fread(&m_ItemInfo[i].DefaultCount, sizeof(UINT), 1, _File);
//		fread(&m_ItemInfo[i].WeightPerCount, sizeof(float), 1, _File);
//		LoadAssetRef(m_ItemInfo[i].UIImage, _File);
//		LoadAssetRef(m_ItemInfo[i].Prefab, _File);
//	}
//}

