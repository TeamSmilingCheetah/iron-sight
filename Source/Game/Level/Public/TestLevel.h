#pragma once

class TestLevel
{
public:
	static void CreateTestLevel();

	static vector<CGameObject*> SetUpUI(CLevel* PLevel);
	static void SetUpPlayer(CLevel* PLevel, const vector<CGameObject*>& PUIInfo);
	static void SetUpEvent(CLevel* PLevel);
};
