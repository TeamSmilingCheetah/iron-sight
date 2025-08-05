#pragma once
#include "Common/singleton.h"

class CLevel;

struct tObjectRefResolution
{
	CGameObject*& MissingAddress;
	UINT			ObjectID;
};

class CLevelMgr :
	public singleton<CLevelMgr>
{
	SINGLE(CLevelMgr)
	friend class CTaskMgr;

private:
	CLevel* m_CurLevel;

public:
	CLevel* GetCurrentLevel() { return m_CurLevel; }
	void RegisterObject(class CGameObject* _Object);
	CGameObject* FindObjectByName(const wstring& _Name);
	CGameObject* FindObjectByID(UINT _ID);
	void ChangeLevelState(LEVEL_STATE _NextState);
	void ChangeLevel(CLevel* _NextLevel, LEVEL_STATE _NextLevelState);

public:
	void Init();
	void Progress();

	// Level Save Load
private:
	vector<tObjectRefResolution>		m_ReferenceResolution;

public:
	static int SaveLevel(const wstring& _FilePath, CLevel* _Level);
	static int SaveGameObject(CGameObject* _Object, FILE* _File);

	static CLevel* LoadLevel(const wstring& _FilePath);
	static CGameObject* LoadGameObject(FILE* _File);

	void ResolveReference(CLevel* _Level);
	void AddObjectRefResolution(CGameObject*& _MissingAddress, UINT _ObjectID);

private:
	static class CComponent* CreateComponent(COMPONENT_TYPE _Type);

};
