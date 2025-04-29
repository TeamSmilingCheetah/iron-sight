#pragma once
#include "Common/singleton.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"

// 구체적인 아이템 타입.
enum class ITEM_TYPE
{
	AKM,					// AKM	

	FIRST_AID_KIT,			// 의료용 키트
	MED_KIT,				// 구급 상자
	BANDAGE,				// 붕대
	ADRENALINE_SYRINGE,		// 아드레날린 주사
	PAIN_KILLER,			// 진통제
	ENERGY_DRINK,			// 에너지 드링크

	GRENADE,				// 수류탄
	MOLOTOV,				// 화염병
	SMOKEBOMB,				// 연막탄
	FLASHBOMB,				// 섬광탄

	AMMO_9,					// 9mm
	AMMO_5,					// 5.56mm
	AMMO_7,					// 7.62mm

	END,
};

// 범위 매크로 : 무기 추가되면 수동으로 수정해야함
#define WEAPON_PRIMARY_BEGIN	static_cast<UINT>(ITEM_TYPE::AKM)
#define WEAPON_PRIMARY_END		static_cast<UINT>(ITEM_TYPE::AKM)
#define WEAPON_SECONDARY_BEGIN	static_cast<UINT>(ITEM_TYPE::AKM)
#define WEAPON_SECONDARY_END	static_cast<UINT>(ITEM_TYPE::AKM)
#define HEAL_BEGIN				static_cast<UINT>(ITEM_TYPE::FIRST_AID_KIT)
#define HEAL_END				static_cast<UINT>(ITEM_TYPE::BANDAGE)
#define BOOST_BEGIN				static_cast<UINT>(ITEM_TYPE::ADRENALINE_SYRINGE)
#define BOOST_END				static_cast<UINT>(ITEM_TYPE::ENERGY_DRINK)
#define THROWABLE_BEGIN			static_cast<UINT>(ITEM_TYPE::GRENADE)
#define THROWABLE_END			static_cast<UINT>(ITEM_TYPE::FLASHBOMB)
#define AMMO_BEGIN				static_cast<UINT>(ITEM_TYPE::AMMO_9)
#define AMMO_END				static_cast<UINT>(ITEM_TYPE::AMMO_7)

#define IS_WEAPON(type) WEAPON_PRIMARY_BEGIN <= type && type <= WEAPON_SECONDARY_END
#define IS_WEAPON_PRIMARY(type) WEAPON_PRIMARY_BEGIN <= type && type <= WEAPON_PRIMARY_END
#define IS_WEAPON_SECONDARY(type) WEAPON_SECONDARY_BEGIN <= type && type <= WEAPON_SECONDARY_END
#define IS_HEAL(type) HEAL_BEGIN <= type && type <= HEAL_END
#define IS_BOOST(type) BOOST_BEGIN <= type && type <= BOOST_END
#define IS_THROWABLE(type) THROWABLE_BEGIN <= type && type <= THROWABLE_END
#define IS_AMMO(type) AMMO_BEGIN <= type && type <= AMMO_END

struct tItemCommonInfo
{
	wstring			Name;
	int				DefaultCount;		// 기본 개수
	int				MaxCount;			// 아이템 스택의 최대 개수
	float			WeightPerCount;		// 1개 당 무게
	Ptr<CTexture>	UIImage;			// UI에 표시될 이미지
	Ptr<CPrefab>	Prefab;				// Prefab
};

struct tItemIndividualInfo
{
	ITEM_TYPE		Type;
	int				Count;
};

class ItemMgr
	: public singleton<ItemMgr>
{
	SINGLE(ItemMgr);

private:
	tItemCommonInfo	m_ItemInfo[static_cast<UINT>(ITEM_TYPE::END)];

public:
	const tItemCommonInfo& GetItemInfo(ITEM_TYPE _Type) { return m_ItemInfo[static_cast<UINT>(_Type)]; }

public:
	void Init();
};
