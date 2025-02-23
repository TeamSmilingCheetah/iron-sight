#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"

CLayer::CLayer()
    : m_LayerIdx(-1)
{
}

CLayer::CLayer(const CLayer& _Origin)
    : CEntity(_Origin)
      , m_LayerIdx(_Origin.m_LayerIdx)
{
    for (size_t i = 0; i < _Origin.m_vecParentObjects.size(); ++i)
    {
        AddObject(_Origin.m_vecParentObjects[i]->Clone(), false);
    }
}

CLayer::~CLayer()
{
    DeleteVec(m_vecParentObjects);
}

void CLayer::Begin()
{
    for (size_t i = 0; i < m_vecParentObjects.size(); ++i)
    {
        m_vecParentObjects[i]->Begin();
    }
}

void CLayer::Tick()
{
    for (size_t i = 0; i < m_vecParentObjects.size(); ++i)
    {
        m_vecParentObjects[i]->Tick();
    }
}

void CLayer::FinalTick()
{
    auto iter = m_vecParentObjects.begin();

    for (; iter != m_vecParentObjects.end();)
    {
        (*iter)->FinalTick();

        if ((*iter)->IsDead())
        {
            iter = m_vecParentObjects.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void CLayer::AddObject(CGameObject* _Object, bool _MoveWithChild)
{
    // 1. _Object �� ������ �ڽĵ鵵 ���� �ش� ���̾ �̵��ϴ� ���
    // 2. _Object �� ������ �ڽĵ��� ���� ���̾ �����ϴ� ���
    // 2-1 _Object �� ������ �ڽĵ� �߿��� ���̾ �Ҽӵ��� ���� ��� 
    //	   �θ� ������Ʈ�� ���󰡰� �Ѵ�.
    static list<CGameObject*> queue;
    queue.clear();
    queue.push_back(_Object);

    CGameObject* pObject = nullptr;
    while (!queue.empty())
    {
        pObject = queue.front();
        queue.pop_front();

        const vector<CGameObject*>& vecChild = pObject->GetChild();
        for (size_t i = 0; i < vecChild.size(); ++i)
        {
            queue.push_back(vecChild[i]);
        }

        // �ֻ��� �θ������Ʈ�� ���
        if (pObject == _Object)
        {
            m_vecParentObjects.push_back(pObject);
            pObject->m_LayerIdx = m_LayerIdx;
        }
        else if (_MoveWithChild || -1 == pObject->m_LayerIdx)
        {
            pObject->m_LayerIdx = m_LayerIdx;
        }
    }
}


void CLayer::RegisterAsParent(CGameObject* _Object)
{
    if (_Object->GetParent() || _Object->GetLayerIdx() != m_LayerIdx)
        return;

    for (size_t i = 0; i < m_vecParentObjects.size(); ++i)
    {
        if (m_vecParentObjects[i] == _Object)
            return;
    }

    m_vecParentObjects.push_back(_Object);
}

void CLayer::DisconnectObject(CGameObject* _Object)
{
    if (nullptr == _Object->GetParent())
    {
        auto iter = m_vecParentObjects.begin();

        for (; iter != m_vecParentObjects.end(); ++iter)
        {
            if (*iter == _Object)
            {
                m_vecParentObjects.erase(iter);
                _Object->m_LayerIdx = -1;
                return;
            }
        }

        assert(nullptr);
    }

    _Object->m_LayerIdx = -1;
}
