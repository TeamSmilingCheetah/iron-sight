#pragma once
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Gameplay/Character/Public/CameraController.h"

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

	CGameObject* m_Camera;
	CameraController* m_CameraScript;
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
	CLONE(GunController);
	GunController();
	~GunController() override;
};

