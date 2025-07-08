#pragma once

class TestLevel
{
public:
	static void CreateTestLevel();

	static void SetUpLight(CLevel* PLevel);
	static void SetUpSkyBox(CLevel* PLevel);
	static void SetUpLandscape(CLevel* PLevel);
	static vector<CGameObject*> SetUpUI(CLevel* PLevel);
	static void SetUpPlayer(CLevel* PLevel, vector<CGameObject*>& PUIInfo);

	static void SetUpWeapon(CLevel* PLevel);
	static void SetUpGrenade(CLevel* PLevel);
	static CGameObject* SetupFBX(CLevel* PLevel, const wstring& PFilePath, const wstring& PName, const Vec3& PPos,
	              const Vec3& PScale, const Vec3& PRotation, const vector<std::function<void(CGameObject*)>>& ComponentSetups,
	              int PLayer, bool PMoveWithChild);
	static void MakeClone(CLevel* PLevel, CGameObject* POrigin, int PLayer, bool PMoveWithChild, int PCopyNum);
};
