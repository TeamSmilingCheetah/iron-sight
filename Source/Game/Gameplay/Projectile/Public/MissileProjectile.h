#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class CLandScape;


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

	void SetBulletInfo(CGameObject* _Object, float _Dmg) { m_BulletDmg.m_Owner = _Object, m_BulletDmg.m_Dmg = _Dmg; }

	void BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,
		CCollider3D* _OtherCollider) override;
	void Overlap(CCollider3D* _Collider, CGameObject* _OtherObject,
		CCollider3D* _OtherCollider) override;
	void EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,
		CCollider3D* _OtherCollider) override;

	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;

	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;


	CLONE(MissileProjectile);
	MissileProjectile();
	~MissileProjectile() override;
};
