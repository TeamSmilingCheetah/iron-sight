#pragma once
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Character/Public/CameraController.h"

class PlayerCharacter;
class InventoryController;

class GunController :
	public WeaponController
{
private:
	Ptr<CSound> m_AkSound;
	int			m_AkSoundIdx;

	float m_HorizontalRecoilPower;
	float m_VerticalRecoilPower;

	float m_InitFirePower;
	float m_BulletDmg;

	float m_FireDelay;
	float m_ReloadingTime;
	float m_AccTime_Fire;
	float m_AccTime_Reload;

	int m_CurRounds;
	int m_MaxRounds;

	bool m_bFire;
	bool m_bReload;
	bool m_bAuto;

	PlayerCharacter*		m_PlayerScript;
	InventoryController*	m_InventoryScript;
	CGameObject*			m_ReloadUI;

	ITEM_TYPE m_WeaponRoundType;

public:
	void Begin() override;
	void Tick() override;


public:
	float GetHorizontalPower() { return m_HorizontalRecoilPower; }
	float GetVerticalPower() { return m_VerticalRecoilPower; }
	float GetFireDelay() { return m_FireDelay; }

	int GetCurRound() { return m_CurRounds; }
	int GetMaxRound() { return m_MaxRounds; }

	bool IsReload() { return m_bReload; }
	bool IsFire() { return m_bFire; }
	bool IsAuto() { return m_bAuto; }

	ITEM_TYPE GetRoundType() { return m_WeaponRoundType; }
private:
	void Firing();
	void Reload();

public:
	SET_PARENT_SCRIPT(SCRIPT_TYPE::WEAPONSCRIPT);
	CLONE(GunController);
	GunController();
	~GunController() override;
};

