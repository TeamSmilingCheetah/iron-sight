#pragma once
#include "Game/Gameplay/Character/Public/EnemyController.h"

class TestCharacter :
    public EnemyController
{
private:
	CGameObject* m_VisionObj;
	class EnemyVisionScript* m_VisionScript;
	Ptr<CSound> m_BulletSound;
	int			m_BulletSoundIdx;

public:
	void Begin() override;
	void Tick() override;

	virtual void DemageCalcul(int _Demage) override;

	void DeathEntry();

public:
	void None();
	void Attack();
	void Trace();
	void Death();

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	CLONE(TestCharacter);
	TestCharacter();
	~TestCharacter() override;
};

