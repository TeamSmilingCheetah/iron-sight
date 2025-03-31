#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class GunController :
	public CScript
{
private:
	CGameObject* m_EquippedOwner;

	float m_HorizontalRecoilPower;
	float m_VerticalRecoilPower;

	float m_InitFirePower;

	float m_FireDelay;
	float m_AccTime;

	bool m_bFire;


public:
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	void SetEquippedOwner(CGameObject* _Owner) { m_EquippedOwner = _Owner; }
	void SetFire(bool _Fire) { m_bFire = _Fire; }

	float GetHorizontalPower() { return m_HorizontalRecoilPower; }
	float GetVerticalPower() { return m_VerticalRecoilPower; }

private:
	void Firing();

public:
	CLONE(GunController);
	GunController();
	~GunController() override;
};
