#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class FLandscape;


class MissileProjectile :
	public CScript
{
private:
	BULLETINFO m_BulletDmg;

	Vec3 m_Velocity;
	Vec3 m_Dir;

	float m_Speed;
	float m_GravityAccel;
	float m_Mass;

	float m_LifeTime;

public:
	void SetVelocity(Vec3 _Velocity) { m_Velocity = _Velocity; }
	Vec3 GetVelocity() { return m_Velocity; }

	void SetSpeed(float _Speed) { m_Speed = _Speed; }
	void SetDir(Vec3 _Dir) { m_Dir = _Dir; }

	void SetBulletInfo(CGameObject* _Object, CGameObject* _Weapon, float _Dmg)
	{
		m_BulletDmg.m_Owner = _Object, m_BulletDmg.m_ShotWeapon = _Weapon, m_BulletDmg.m_Dmg = _Dmg;
	}

	void BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;
	void Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override {}
	void EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override {}

	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;


	CLONE(MissileProjectile);
	MissileProjectile();
	~MissileProjectile() override;
};
