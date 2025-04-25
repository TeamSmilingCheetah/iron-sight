#pragma once
#include "Game/Gameplay/Weapon/Public/WeaponController.h"


class CLandScape;

class ThrowableController :
	public WeaponController
{
private:
	Vec3 m_Velocity;
	Vec3 m_Dir;

	float m_Mass;
	float m_Speed;
	float m_GravityAccel;
	float m_AccTime;
	float m_TriggeredTime;
	float m_ThrowAngle;


	bool m_bGround;
	bool m_bCanThrow;
	bool m_bThrow;
	bool m_bTrigger;
	

public:
	void Begin() override;
	void Tick() override;


	void Triggered();
	void Throw();

	void MakeBounce(Vec3 _Normal, float _elastic, float _friction);
public:
	void BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,
		CCollider3D* _OtherCollider) override;
	void Overlap(CCollider3D* _Collider, CGameObject* _OtherObject,
		CCollider3D* _OtherCollider) override;
	void EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,
		CCollider3D* _OtherCollider) override;


	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;


public:
	CLONE(ThrowableController);
	ThrowableController();
	~ThrowableController() override;
};

