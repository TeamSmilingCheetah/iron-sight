#pragma once
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Character/Public/CameraController.h"

class PlayerCharacter;

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

	PlayerCharacter* m_PlayerScript;

public:
	void Begin() override;
	void Tick() override;


public:
	float GetHorizontalPower() { return m_HorizontalRecoilPower; }
	float GetVerticalPower() { return m_VerticalRecoilPower; }

	int GetCurRound() { return m_CurRounds; }

	bool IsReload() { return m_bReload; }
	bool IsFire() { return m_bFire; }

private:
	void Firing();
	void Reload();

public:
	SET_PARENT_SCRIPT(SCRIPT_TYPE::WEAPONSCRIPT);
	CLONE(GunController);
	GunController();
	~GunController() override;
};

