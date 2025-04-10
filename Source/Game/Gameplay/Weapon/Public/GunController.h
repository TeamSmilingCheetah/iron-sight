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
	float m_AccTime;

	bool m_bFire;
public:
	void Begin() override;
	void Tick() override;

public:
	float GetHorizontalPower() { return m_HorizontalRecoilPower; }
	float GetVerticalPower() { return m_VerticalRecoilPower; }

private:
	void Firing();

public:
	CLONE(GunController);
	GunController();
	~GunController() override;
};

