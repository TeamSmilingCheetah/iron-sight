#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"

enum class WEAPON_TYPE
{
	PRIMARY,
	SECONDARY,
	THROWABLE,
};


class WeaponController :
	public CScript
{
protected:
	CGameObject* m_MainCamera;
	CGameObject* m_EquippedOwner;
	WEAPON_TYPE m_WeaponType;

	KEY m_CurKey;
	KEY_STATE m_CurKeyState;

	bool m_bIsEquipped;
	bool m_bEnemy;

public:
	void Begin() override;
	void Tick() override = 0;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	void SetEquippedOwner(CGameObject* _Owner) { m_EquippedOwner = _Owner; }
	void SetWeaponType(WEAPON_TYPE _Type) { m_WeaponType = _Type; }

	void SetCurKey(KEY _Key) { m_CurKey = _Key; }
	void SetCurKeyState(KEY_STATE _KeyState) { m_CurKeyState = _KeyState; }
	void SetEquip(bool _Equip) { m_bIsEquipped = _Equip; }
	void SetEquipEnemy(bool _Enemy) { m_bEnemy = _Enemy; }

	CGameObject* GetEquippedOwner() const { return m_EquippedOwner; }

	Vec3 GetFireDir();

	void ClearKey();

	WEAPON_TYPE GetWeaponType() { return m_WeaponType; }

public:
	//CLONE(WeaponController);
	WeaponController(SCRIPT_TYPE _Type);
	~WeaponController() override;
};
