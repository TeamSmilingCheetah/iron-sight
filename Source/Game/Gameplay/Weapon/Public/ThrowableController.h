#pragma once
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

class FLandScape;

class ThrowableController :
	public WeaponController
{
private:
	Ptr<CSound> m_BombSound;
	Ptr<CSound> m_SmokeSound;
	Ptr<CSound> m_PinSound;
	Ptr<CSound> m_ThrowSound;
	Ptr<CSound> m_BounceSound_1;
	Ptr<CSound> m_BounceSound_2;

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
	bool m_bThrow;
	bool m_bAfterThrow;
	bool m_bTrigger;
	bool m_bUseFirstBounceSound;

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

	virtual void BeginOverlap(FCollider3D* _Collider, CGameObject* _OtherObject, FCollider3D* _OtherCollider) override;
	virtual void Overlap(FCollider3D* _Collider, CGameObject* _OtherObject, FCollider3D* _OtherCollider) override;
	virtual void EndOverlap(FCollider3D* _Collider, CGameObject* _OtherObject, FCollider3D* _OtherCollider) override;

	virtual void BeginOverlap(class FCollider3D* _Collider, CGameObject* _OtherObject, FLandScape* _OtherCollider) override;
	virtual void Overlap(class FCollider3D* _Collider, CGameObject* _OtherObject, FLandScape* _OtherCollider) override;
	virtual void EndOverlap(class FCollider3D* _Collider, CGameObject* _OtherObject, FLandScape* _OtherCollider) override;


public:
	SET_PARENT_SCRIPT(SCRIPT_TYPE::WEAPONSCRIPT)
	CLONE(ThrowableController)
	ThrowableController();
	~ThrowableController() override;
};

