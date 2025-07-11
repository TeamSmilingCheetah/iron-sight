#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

struct EnemyDot
{
	CGameObject* Enemy;
	CGameObject* Dot;
};

class MinimapUIScript :
    public CScript
{
private:
	CGameObject* m_Player;

	CGameObject* m_PlayerDot;

	vector<EnemyDot> m_EnemyDots;

	float m_MinimapRange;

public:
	void Begin() override;
	void Tick() override;

	void UpdatePlayerDot();
	void UpdateEnemyDots();

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	CLONE(MinimapUIScript);
	MinimapUIScript();
	~MinimapUIScript() override;
};

