#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class ParticleController :
	public CScript
{
private:
	class CParticleSystem* m_ParticleSystem;

	float m_LifeTime;
	float m_CurClipAccTime;

	bool m_SpawnTrigger;

public:
	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:


public:
	CLONE(ParticleController);
	ParticleController();
	~ParticleController() override;
};

