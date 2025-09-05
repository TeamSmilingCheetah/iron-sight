#pragma once
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Character/Public/CameraController.h"


struct RecoilPatternStep
{
	float pitch;
	float yaw;
	float time;
};


class PlayerCharacter;
class InventoryController;

class GunController :
	public WeaponController
{
private:
	Ptr<FSound> m_AkSound;
	Ptr<FSound> m_AkdrySound;
	Ptr<FSound> m_EmptyReloadSound;
	Ptr<FSound> m_ReloadSound;
	Ptr<FSound> m_ClipSound;
	
	int			m_AkSoundIdx;
	int			m_AkdrySoundIdx;
	int			m_EmptyReloadSoundIdx;
	int			m_ReloadSoundIdx;
	int			m_ClipSoundIdx;


	float m_InitFirePower;
	float m_BulletDmg;

	float m_SingleRecoilPower;
	float m_FireDelay;
	float m_ReloadingTime;
	float m_AccTime_Fire;
	float m_AccTime_Reload;

	int m_CurRounds;
	int m_MaxRounds;
	int m_leftRounds;

	bool m_bPullTrigger;
	bool m_bFire;
	bool m_bReload;
	bool m_bAuto;

	vector<RecoilPatternStep> m_vecRecoilPattern;

	PlayerCharacter*		m_PlayerScript;
	InventoryController*	m_InventoryScript;

	ITEM_TYPE m_WeaponRoundType;
public:
	void Begin() override;
	void Tick() override;


public:
	float GetSingleRecoilPower() const { return m_SingleRecoilPower; }
	float GetFireDelay() const { return m_FireDelay; }
	float GetReloadingTime() const { return m_ReloadingTime; }
	vector<RecoilPatternStep> GetRecoilPattern() const { return m_vecRecoilPattern; }

	int GetCurRound() const { return m_CurRounds; }
	int GetMaxRound() const { return m_MaxRounds; }

	bool IsReload() const { return m_bReload; }
	bool IsPullTrigger() const { return m_bPullTrigger; }
	bool IsFire() const { return m_bFire; }
	bool IsAuto() const { return m_bAuto; }

	void SetCurRounds(int _Rounds) { m_CurRounds = _Rounds; }

	ITEM_TYPE GetRoundType() const { return m_WeaponRoundType; }

private:
	void ActiveTrigger();
	void Reload();
	void StopFiring();

	bool CanReload();


	void InitWeaponVariable();
	void InitRecoilPattern(ITEM_TYPE _type);
public:
	SET_PARENT_SCRIPT(SCRIPT_TYPE::WEAPONSCRIPT)
	CLONE(GunController)
	GunController();
	~GunController() override;
};

