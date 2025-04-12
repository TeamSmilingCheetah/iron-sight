#include "pch.h"
#include "Runtime/Public/Actor/CLayer.h"
#include "Runtime/Public/Actor/CGameObject.h"

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
	// 1. _Object 가 소유한 자식들도 같이 해당 레이어에 이동하는 경우
	// 2. _Object 가 소유한 자식들은 본래 레이어를 유지하는 경우
	// 2-1 _Object 가 소유한 자식들 중에서 레이어가 소속되지 않은 경우
	//	   부모 오브젝트를 따라가게 한다.
	queue<CGameObject*> Q;
	Q.emplace(_Object);

	CGameObject* pObject = nullptr;
	while (!Q.empty())
	{
		pObject = Q.front();
		Q.pop();

		const vector<CGameObject*>& vecChild = pObject->GetChild();
		for (size_t i = 0; i < vecChild.size(); ++i)
		{
			Q.emplace(vecChild[i]);
		}

		// 최상위 부모오브젝트인 경우
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
