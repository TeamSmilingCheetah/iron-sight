#include "pch.h"

#include "Runtime/Public/Actor/CGameObject.h"
#include "System/Public/Asset/Prefab/CPrefab.h"
#include "System/Public/Manager/CObjectPoolMgr.h"

CObjectPoolMgr::CObjectPoolMgr()
{
}
CObjectPoolMgr::~CObjectPoolMgr()
{
}

CGameObject* CObjectPoolMgr::GetPoolObject(const wstring& _Name, int _Layer)
{
	Ptr<CPrefab> pPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\" + _Name + L".pref", _Name);

	vector<CGameObject*>& vec = m_mapPool[_Name];

   	if (!vec.empty()) {
		CGameObject* obj = vec.back();
		vec.pop_back();

		SetObjectActive(obj, true);
		ChangeLayer(obj, _Layer);
		return obj;
	}
	else
	{
		CGameObject* obj = pPrefab->Instantiate();
		CreateObject(obj, _Layer, false);
		return obj;
	}
}

void CObjectPoolMgr::ReturnObject(CGameObject* _Obj)
{
	if (!_Obj)
		return;

	SetObjectActive(_Obj, false);
	ChangeLayer(_Obj, 9);
	wstring objName = _Obj->GetName();
	m_mapPool[objName].push_back(_Obj);
	
}

void CObjectPoolMgr::Preload(const wstring& _Name, size_t _Count)
{
	Ptr<CPrefab> pPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\" + _Name + L".pref", _Name);

	// count만큼 Clone & 비활성화 후 저장
	vector<CGameObject*>& pool = m_mapPool[_Name];

	// 벡터 초기화
	pool.clear();
	for (size_t i = 0; i < _Count; ++i)
	{
		CGameObject* obj = pPrefab->GetProtoObject()->Clone();
		SetObjectActive(obj, false);
		obj->SetName(_Name);
		pool.push_back(obj);
		CreateObject(obj, 9, true);
	}
}

void CObjectPoolMgr::Clear()
{
	for (auto& pair : m_mapPool)
	{
		for (CGameObject* obj : pair.second)
		{
			delete obj;
		}
		pair.second.clear();
	}
	m_mapPool.clear();
}
