#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class MissileProjectile :
	public CScript
{
private:
	Vec3 m_Velocity;
	Vec3 m_Dir;

	float m_LifeTime;

public:
	void SetVelocity(Vec3 _Velocity) { m_Velocity = _Velocity; }
	Vec3 GetVelocity() { return m_Velocity; }

	void SetDir(Vec3 _Dir) { m_Dir = _Dir; }
	
	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;


	CLONE(MissileProjectile);
	MissileProjectile();
	~MissileProjectile() override;
};
