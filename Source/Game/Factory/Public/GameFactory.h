#pragma once

class GameFactory
{
public:
	static unique_ptr<CGameObject> CreateObject();
	static unique_ptr<CLevel> CreateLevel();

	static void SetUpLayer(CLevel* PLevel);
	static void SetUpCollision();
	static void SetUpCamera(CLevel* PLevel);

	// void SetUpLight(CLevel* PLevel);
	// void SetUpSkyBox(CLevel* PLevel);
	// void SetUpLandscape(CLevel* PLevel);
	// vector<CGameObject*> SetUpUI(CLevel* PLevel);
	// void SetUpPlayer(CLevel* PLevel, vector<CGameObject*>& PUIInfo);
	//
	// void SetUpWeapon(CLevel* PLevel);
	// void SetUpGrenade(CLevel* PLevel);
	// CGameObject* SetupFBX(CLevel* PLevel, const wstring& PFilePath, const wstring& PName, const Vec3& PPos,
	// 			  const Vec3& PScale, const Vec3& PRotation, const vector<std::function<void(CGameObject*)>>& ComponentSetups,
	// 			  int PLayer, bool PMoveWithChild);
	// void MakeClone(CLevel* PLevel, CGameObject* POrigin, int PLayer, bool PMoveWithChild, int PCopyNum);
};

