#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class RoundsUIScript :
	public CScript
{
private:
	CGameObject* m_Player;
	CGameObject* m_PlayerWeapon;

	class PlayerCharacter* m_PlayerScript;
	class InventoryController* m_InvetoryScript;

public:
	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	CLONE(RoundsUIScript);
	RoundsUIScript();
	~RoundsUIScript() override;
};

