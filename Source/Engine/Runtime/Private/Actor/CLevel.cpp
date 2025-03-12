#include "pch.h"
#include "Runtime/Public/Actor/CLevel.h"
#include "Runtime/Public/Actor/CGameObject.h"

CLevel::CLevel()
    : m_State(LEVEL_STATE::NONE)
      , m_arrLayer{}
{
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        m_arrLayer[i].m_LayerIdx = i;
    }
}

CLevel::~CLevel()
{
}


void CLevel::Begin()
{
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        m_arrLayer[i].Begin();
    }
}

void CLevel::Tick()
{
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        m_arrLayer[i].Tick();
    }
}

void CLevel::FinalTick()
{
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        m_arrLayer[i].FinalTick();
    }
}

void CLevel::AddObject(int _LayerIdx, CGameObject* _Object, bool _MoveWithChild)
{
    m_arrLayer[_LayerIdx].AddObject(_Object, _MoveWithChild);
}


CGameObject* CLevel::FindObjectByName(const wstring& _Name)
{
    for (int i = 0; i < MAX_LAYER; ++i)
    {
        const vector<CGameObject*>& vecParents = m_arrLayer[i].GetParentObjects();

        for (size_t j = 0; j < vecParents.size(); ++j)
        {
			queue<CGameObject*> Q;
            Q.emplace(vecParents[j]);

            while (!Q.empty())
            {
                CGameObject* pObject = Q.front();
                Q.pop();

                if (pObject->GetName() == _Name)
                {
                    return pObject;
                }

                const vector<CGameObject*>& vecChild = pObject->GetChild();
                for (size_t k = 0; k < vecChild.size(); ++k)
                {
                    Q.emplace(vecChild[k]);
                }
            }
        }
    }

    return nullptr;
}

void CLevel::ChangeState(LEVEL_STATE _NextState)
{
    m_State = _NextState;
}
