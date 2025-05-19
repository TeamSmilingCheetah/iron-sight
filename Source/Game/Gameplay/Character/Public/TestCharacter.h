#pragma once
#include "Game/Gameplay/Character/Public/EnemyController.h"

class WeaponController;

class TestCharacter :
    public EnemyController
{
private:
	WeaponController* m_WeaponScript;	// 지니고 있는 무기의 스크립트
	SCRIPT_TYPE m_WeaponType;			// 무기 스크립트 타입

	float m_AttackDeley;		// 발사 간격시간
	float m_AttackTime;			// 발사한 시간
	bool  m_IsAttack;			// 공격 딜레이판정

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

	virtual void KeyInputProcessing();

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	CLONE(TestCharacter);
	TestCharacter();
	~TestCharacter() override;
};

