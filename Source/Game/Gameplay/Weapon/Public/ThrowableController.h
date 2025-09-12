#pragma once
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

class FLandscape;

class ThrowableController :
	public WeaponController
{
private:
	Ptr<FSound> m_BombSound;
	Ptr<FSound> m_SmokeSound;
	Ptr<FSound> m_PinSound;
	Ptr<FSound> m_ThrowSound;
	Ptr<FSound> m_BounceSound_1;
	Ptr<FSound> m_BounceSound_2;

	int			m_BombSoundIdx;
	int			m_SmokeSoundIdx;
	int			m_PinSoundIdx;
	int			m_ThrowSoundIdx;
	int			m_BounceSoundIdx_1;
	int			m_BounceSoundIdx_2;

	Vec3 m_Velocity;
	Vec3 m_Dir;

	float m_Mass;
	float m_Speed;
	float m_GravityAccel;
	float m_CurClipAccTime;
	float m_TriggeredTime;
	float m_ThrowAngle;

	bool m_bGround;
	bool m_bCanThrow;
	bool m_bHighThrow;
	bool m_bLowThrow;
	bool m_bThrow;
	bool m_bAfterThrow;
	bool m_bTrigger;
	bool m_bUseFirstBounceSound;
	bool m_bThrowTriggered;

	CGameObject* m_ThownOwner;
	CGameObject* m_Player;
	PlayerCharacter* m_PlayerScript;

	float m_AfterThrowAccTime;
	const float m_ActionEndTime = 1.7f;

private:
	void Triggered();
	void Throw();

	void MakeBounce(Vec3 _Normal, float _elastic, float _friction);

public:
	void Begin() override;
	void Tick() override;

	void BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;
	void Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;
	void EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;


	bool IsThrow() { return m_bThrow; }
	bool IsAfterThrow() { return m_bAfterThrow; }
	float GetActionEndTime() const { return m_ActionEndTime; }

public:
	SET_PARENT_SCRIPT(SCRIPT_TYPE::WEAPONSCRIPT)
	CLONE(ThrowableController)
	ThrowableController();
	~ThrowableController() override;
};

