#pragma once


class CGameObject;

class CObjectPoolMgr 
	: public singleton<CObjectPoolMgr>
{
	SINGLE(CObjectPoolMgr);

private:
	unordered_map<wstring, vector<CGameObject*>> m_mapPool;

public:
	CGameObject* GetPoolObject(const wstring& _Name, int _Layer);
	void ReturnObject(CGameObject* _Obj);
	void Preload(const wstring& _Name, size_t _Count);
	void Clear();  
};
