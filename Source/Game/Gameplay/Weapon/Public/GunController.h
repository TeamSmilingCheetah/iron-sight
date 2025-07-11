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
	float GetHorizontalPower() const { return m_HorizontalRecoilPower; }
	float GetVerticalPower() const { return m_VerticalRecoilPower; }
	float GetFireDelay() const { return m_FireDelay; }

	int GetCurRound() const { return m_CurRounds; }
	int GetMaxRound() const { return m_MaxRounds; }

	bool IsReload() const { return m_bReload; }
	bool IsFire() const { return m_bFire; }
	bool IsAuto() const { return m_bAuto; }

	ITEM_TYPE GetRoundType() const { return m_WeaponRoundType; }

private:
	void Firing();
	void Reload();
	void StopFiring();

public:
	SET_PARENT_SCRIPT(SCRIPT_TYPE::WEAPONSCRIPT);
	CLONE(GunController);
	GunController();
	~GunController() override;
};

