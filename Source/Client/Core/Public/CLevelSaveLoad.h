#pragma once

struct tObjectRefResolution
{
	CGameObject*&	MissingAddress;
	UINT			ObjectID;
};

class CLevelSaveLoad
	: public singleton<CLevelSaveLoad>
{
	SINGLE(CLevelSaveLoad);

private:
	vector<tObjectRefResolution>		m_ReferenceResolution;

public:
	void ResolveReference(CLevel* _Level);
	void AddObjectRefResolution(CGameObject*& _MissingAddress, UINT _ObjectID);

public:
	static int SaveLevel(const wstring& _FilePath, CLevel* _Level);
	static int SaveGameObject(CGameObject* _Object, FILE* _File);

	static CLevel* LoadLevel(const wstring& _FilePath);
	static CGameObject* LoadGameObject(FILE* _File);

private:
	static class CComponent* CreateComponent(COMPONENT_TYPE _Type);
};
