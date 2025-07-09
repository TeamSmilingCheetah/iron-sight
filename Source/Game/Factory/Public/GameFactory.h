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
	static void MakeCloneObject(CLevel* PLevel, CGameObject* POrigin, bool PMoveWithChild, int PCopyMount);

	// TODO(KHJ): 임시 Return, 전부 Engine 지원 함수 통해서 Add 할 수 있도록 처리할 것
	static CGameObject* LoadDefaultPlayer(CLevel* PLevel, const Vec3& PPosition);
	static CGameObject* MakeFBXObject(CLevel* PLevel, const wstring& PFilePath, const wstring& PName,
	                                  const Vec3& PPosition, const Vec3& PRotation, const Vec3& PScale,
	                                  const vector<std::function<void(CGameObject*)>>& PComponentSetups,
	                                  int PLayer, bool PMoveWithChild);


	// vector<CGameObject*> SetUpUI(CLevel* PLevel);
};
