#pragma once

class GameFactory
{
public:
	static unique_ptr<CGameObject> CreateObject();
	static unique_ptr<CLevel> CreateLevel();

	static void LoadDefaultLayer(CLevel* PLevel);
	static void LoadDefaultCollisionSetting();
	static void LoadMainCamera(CLevel* PLevel);
	static void LoadDefaultLight(CLevel* PLevel);
	static void LoadDefaultSkyBox(CLevel* PLevel);
	static void LoadDefaultLandscape(CLevel* PLevel);

	static CGameObject* MakeFBXObject(CLevel* PLevel, const wstring& PFilePath, const wstring& PName,
	                                  const Vec3& PPosition, const Vec3& PRotation, const Vec3& PScale,
	                                  const vector<std::function<void(CGameObject*)>>& PComponentSetups,
	                                  int PLayer, bool PMoveWithChild);

	static void MakeCloneObject(CLevel* PLevel, CGameObject* POrigin, bool PMoveWithChild, int PCopyMount);

	// vector<CGameObject*> SetUpUI(CLevel* PLevel);
	// void SetUpPlayer(CLevel* PLevel, vector<CGameObject*>& PUIInfo);
};
