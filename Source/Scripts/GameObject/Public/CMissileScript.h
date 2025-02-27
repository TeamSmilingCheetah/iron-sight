#pragma once
#include "Runtime/Public/Component/Script/CScript.h"

class CMissileScript :
	public CScript
{
	Vec3 m_Velocity;

public:
	void SetVelocity(Vec3 _Velocity) { m_Velocity = _Velocity; }
	Vec3 GetVelocity() { return m_Velocity; }

	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	CLONE(CMissileScript);
	CMissileScript();
	~CMissileScript() override;
};
