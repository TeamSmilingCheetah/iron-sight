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

// TODO(KHJ): 포인터 사용에 대해 일원화할 것
void CLevel::AddObject(int _LayerIdx, CGameObject* _Object, bool _MoveWithChild)
{
	m_arrLayer[_LayerIdx].AddObject(_Object, _MoveWithChild);
}

void CLevel::AddObject(int _LayerIdx, unique_ptr<CGameObject> _Object, bool _MoveWithChild)
{
	m_arrLayer[_LayerIdx].AddObject(_Object.release(), _MoveWithChild);
}

/**
 * 레벨 내의 오브젝트를 전체 탐색하여 해당 이름을 가진 오브젝트를 반환하는 함수
 *
 * @param _Name [IN] 오브젝트의 이름
 * @return 해당 이름을 가진 오브젝트 포인터를 반환
 */
CGameObject* CLevel::FindObjectByName(const wstring& _Name)
{
	queue<CGameObject*> Q;
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		for (CGameObject* pParent : m_arrLayer[i].GetParentObjects())
		{
			Q.emplace(pParent);
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

CGameObject* CLevel::FindObjectByObjectID(UINT _ObjectID)
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

				if (pObject->GetObjectID() == _ObjectID)
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

/**
 * @brief Level에 존재하는 오브젝트들을 탐색해서 제공하는 함수
 * @param PObjects [OUT] 오브젝트를 Layer 단위로 분류해서 넣을 Vector
 */
void CLevel::GetAllActiveObjectsInLevel(vector<CGameObject*>& PObjects)
{
	queue<CGameObject*> GameObjectQueue;

	for (int i = 0; i < MAX_LAYER; ++i)
	{
		for (auto* Parent : m_arrLayer[i].GetParentObjects())
		{
			GameObjectQueue.push(Parent);

			// BFS
			while (!GameObjectQueue.empty())
			{
				CGameObject* Object = GameObjectQueue.front();
				GameObjectQueue.pop();

				// Active가 아니고 Dead 상태인 Object의 자식을 Recursive Add할 필요도 없음
				if (!Object->IsActive() || Object->IsDead())
				{
					continue;
				}

				PObjects.push_back(Object);

				// Add Child
				for (auto* Child : Object->GetChild())
				{
					GameObjectQueue.push(Child);
				}
			}
		}
	}
}
