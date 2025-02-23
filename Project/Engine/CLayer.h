#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{
    vector<CGameObject*> m_vecParentObjects; // Layer �Ҽ� ������Ʈ && �ֻ��� �θ�
    vector<CGameObject*> m_vecObjects; // Layer �Ҽ� ������Ʈ (�� �����Ӹ��� ��Ϲ޴� ����)
    int m_LayerIdx; // Level �� �����ִ� Layer �� �ε���

public:
    const vector<CGameObject*>& GetParentObjects() { return m_vecParentObjects; }
    const vector<CGameObject*>& GetObjects() { return m_vecObjects; }

    void RegisterAsParent(CGameObject* _Object);

    void RegisterObject(CGameObject* _Object) { m_vecObjects.push_back(_Object); }
    void DisconnectObject(CGameObject* _Object);


    void AddObject(CGameObject* _Object, bool _MoveWithChild);

    void Begin();
    void Tick();
    void FinalTick();

    CLONE(CLayer);
    CLayer();
    CLayer(const CLayer& _Origin);
    ~CLayer() override;

    friend class CLevel;
};
