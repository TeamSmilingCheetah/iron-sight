#pragma once
#include "Game/Gameplay/Weapon/Public/WeaponController.h"

class GunController :
	public WeaponController
{
private:
	Ptr<CSound> m_AkSound;
	int			m_AkSoundIdx;

	float m_HorizontalRecoilPower;
	float m_VerticalRecoilPower;

	float m_InitFirePower;

	float m_FireDelay;
	float m_ReloadingTime;
	float m_AccTime_Fire;
	float m_AccTime_Reload;

	int m_CurRounds;
	int m_MaxRounds;

	bool m_bFire;
	bool m_bReload;


public:
	void Begin() override;
	void Tick() override;

public:
	float GetHorizontalPower() { return m_HorizontalRecoilPower; }
	float GetVerticalPower() { return m_VerticalRecoilPower; }

private:
	void Firing();
	void Reload();

public:
	CLONE(GunController);
	GunController();
	~GunController() override;
};

