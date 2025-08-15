#include "pch.h"
#include "Runtime/Public/Actor/CLevel.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "Runtime/Public/Component/Physics/ColliderBase.h"

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
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		for (auto* Object : m_arrLayer[i].GetObjects())
		{
			if (Object->IsActive())
			{
				PObjects.push_back(Object);
			}
		}
	}
}

/**
 * @brief Level 시작 시점에 Manager가 관리할 수 있도록 움직임 유무에 따라 충돌체를 분류해서 제공하는 함수
 * 여기서부터 Ray는 따로 분류해서 처리한다
 * @param InStatic 정적인 충돌체를 모아놓을 Vector
 * @param InDynamic 움직이는 충돌체를 모아놓을 Vector
 * @param InRays Ray만 모아놓은 Vector
 */
void CLevel::GetAllCollidersInLevel(vector<IColliderBase*>& InStatic,
                                    vector<IColliderBase*>& InDynamic, vector<FRayCollider*>& InRays)
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		for (auto* Object : m_arrLayer[i].GetObjects())
		{
			if (Object->IsActive())
			{
				for (auto Variant : Object->GetColliders())
				{
					IColliderBase* Collider = GetBaseFromVariant(Variant);

					if (Collider->GetColliderType() == EColliderType::ColliderRay)
					{
						InRays.push_back(reinterpret_cast<FRayCollider*>(Collider));
						continue;
					}

					if (Collider->IsStatic())
					{
						InStatic.push_back(Collider);
					}
					else
					{
						InDynamic.push_back(Collider);
					}
				}
			}
		}
	}
}

/**
 * @brief Level 중간 시점에 충돌체를 분류해서 제공하는 함수
 * Static Collider는 다시 쌓지 않는다
 * @param InDynamic 움직이는 충돌체를 모아놓을 Vector
 * @param InRays Ray만 모아놓은 Vector
 */
void CLevel::GetDynamicCollidersInLevel(vector<IColliderBase*>& InDynamic, vector<FRayCollider*>& InRays)
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		for (auto* Object : m_arrLayer[i].GetObjects())
		{
			if (Object->IsActive())
			{
				for (auto Variant : Object->GetColliders())
				{
					IColliderBase* Collider = GetBaseFromVariant(Variant);

					if (Collider->GetColliderType() == EColliderType::ColliderRay)
					{
						InRays.push_back(reinterpret_cast<FRayCollider*>(Collider));
						continue;
					}

					if (!Collider->IsStatic())
					{
						InDynamic.push_back(Collider);
					}
				}
			}
		}
	}
}
