#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class MinimapCameraScript :
    public CScript
{
private:
	CGameObject* m_Player;

	class PlayerCharacter* m_PlayerScript;

public:
	void Begin() override;
	void Tick() override;

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	CLONE(MinimapCameraScript);
	MinimapCameraScript();
	~MinimapCameraScript() override;
};

