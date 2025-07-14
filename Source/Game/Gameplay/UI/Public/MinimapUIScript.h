#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

struct EnemyDot
{
	CGameObject* Enemy;
	CGameObject* Dot;
};

struct DeadEnemyInfo
{
	UINT EnemyID;		// 적 오브젝트 ID
	Vec3 WorldPos;      // 죽은 위치
	CGameObject* Dot;   // UI 점
};

class MinimapUIScript :
    public CScript
{
private:
	CGameObject* m_Player;

	CGameObject* m_PlayerDot;

	vector<EnemyDot> m_EnemyDots;
	CGameObject* m_PlayerArrow;    // 플레이어 방향 삼각형

	vector<EnemyDot> m_EnemyDots;
	vector<DeadEnemyInfo> m_DeadEnemies;  // 죽은 적들 정보
	set<UINT> m_ProcessedDeadEnemyIDs;

	float m_MinimapRange;

public:
	void Begin() override;
	void Tick() override;

	void UpdatePlayerDot();
	void UpdateEnemyDots();

	void CheckForNewDeadEnemies();  // 새로 죽은 적들 감지
	void UpdateDeadEnemies();

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	CLONE(MinimapUIScript);
	MinimapUIScript();
	~MinimapUIScript() override;
};

