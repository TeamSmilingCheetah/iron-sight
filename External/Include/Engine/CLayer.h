#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{
private:
	vector<CGameObject*>	m_vecParentObjects; // Layer 소속 오브젝트 && 최상위 부모
	vector<CGameObject*>	m_vecObjects;		// Layer 소속 오브젝트 (매 프레임마다 등록받는 구조)
	int						m_LayerIdx;			// Level 에 속해있는 Layer 의 인덱스

public:
	const vector<CGameObject*>& GetParentObjects() { return m_vecParentObjects; }
	const vector<CGameObject*>& GetObjects() { return m_vecObjects; }

	void RegisterAsParent(CGameObject* _Object);

	void RegisterObject(CGameObject* _Object) { m_vecObjects.push_back(_Object); }
	void DisconnectObject(CGameObject* _Object);


public:
	void AddObject(CGameObject* _Object, bool _MoveWithChild);

public:
	void Begin();
	void Tick();
	void FinalTick();

public:
	CLONE(CLayer);
	CLayer();
	CLayer(const CLayer& _Origin);
	~CLayer();

	friend class CLevel;
};

