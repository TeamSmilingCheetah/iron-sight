#pragma once
#include "Engine/Core/Public/CEntity.h"
#include "Engine/Runtime/Public/Actor/CLayer.h"

class CLevel
	: public CEntity
{
private:
	LEVEL_STATE m_State;
	CLayer m_arrLayer[MAX_LAYER];

public:
	CLayer* GetLayer(int _LayerIdx) { return &m_arrLayer[_LayerIdx]; }
	CGameObject* FindObjectByName(const wstring& _Name);
	CGameObject* FindObjectByObjectID(UINT _ObjectID);
	LEVEL_STATE GetState() const { return m_State; }
	void GetAllActiveObjectsInLevel(vector<CGameObject*>& PObjects);

	void RegisterClear()
	{
		// 이전 프레임에 등록받았던 오브젝트 목록을 Clear한다.
		for (UINT i = 0; i < MAX_LAYER; ++i)
		{
			m_arrLayer[i].m_vecObjects.clear();
		}
	}

	void Begin();
	void Tick();
	void FinalTick();

	void AddObject(int _LayerIdx, CGameObject* _Object, bool _MoveWithChild);
	void AddObject(int _LayerIdx, unique_ptr<CGameObject> _Object, bool _MoveWithChild);

private:
	void ChangeState(LEVEL_STATE _NextState);

public:
	CLevel* Clone() override { return new CLevel(*this); }
	CLevel();
	~CLevel() override;

	friend class CLevelMgr;
};
