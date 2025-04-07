#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class TestSound :
    public CScript
{
public:
	Ptr<CSound> m_BulletSound;
	int			m_BulletSoundIdx;

public:
	void Begin() override;
	void Tick() override;


	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	CLONE(TestSound);
	TestSound();
	~TestSound() override;
};

