#pragma once
#include "Gameplay/Character/Public/PlayerCharacter.h"
#include "WeaponController.h"

class GunController :
	public WeaponController
{
private:
	Ptr<CSound> m_AkSound;
	Ptr<CSound> m_AkdrySound;
	Ptr<CSound> m_EmptyReloadSound;
	Ptr<CSound> m_ReloadSound;
	Ptr<CSound> m_ClipSound;
	
	int			m_AkSoundIdx;
	int			m_AkdrySoundIdx;
	int			m_EmptyReloadSoundIdx;
	int			m_ReloadSoundIdx;
	int			m_ClipSoundIdx;

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
	int m_leftRounds;

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

	bool CanReload();

public:
	SET_PARENT_SCRIPT(SCRIPT_TYPE::WEAPONSCRIPT);
	CLONE(GunController);
	GunController();
	~GunController() override;
};

