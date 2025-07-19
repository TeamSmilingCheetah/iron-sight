#include "pch.h"
#include "System/Public/Manager/CTaskMgr.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "System/Public/Manager/CAssetMgr.h"
#include "System/Public/Manager/CLevelMgr.h"

class CAsset;

CTaskMgr::CTaskMgr()
	: m_LevelChanged(false)
{
}

CTaskMgr::~CTaskMgr()
{
}

void CTaskMgr::Tick()
{
	m_LevelChanged = false;

	for (size_t i = 0; i < m_vecDelayedTask.size(); ++i)
	{
		const tTask& task = m_vecDelayedTask[i];
		switch (task.Type)
		{
		case TASK_TYPE::ADD_CHILD:
		{
			CGameObject* pParent = (CGameObject*)task.Param0;
			CGameObject* pChild = (CGameObject*)task.Param1;

			// 목적지가 없다
			if (nullptr == pParent)
			{
				if (pChild->GetParent())
				{
					pChild->DisconnecntWithParent();
					pChild->RegisterAsParent();
				}
			}

			// 목적지가 있다.
			else
			{
				if (pChild->GetParent())
					pChild->DisconnecntWithParent();
				else
				{
					int LayerIdx = pChild->GetLayerIdx();
					pChild->DisconnectWithLayer();
					pChild->MLayerIdx = LayerIdx;
				}

				pParent->AddChild(pChild);
			}

			pChild->LayerMoveDone();

			m_LevelChanged = true;
		}
			break;
		case TASK_TYPE::CHANGE_LAYEROBJECT:
		{
			CGameObject* pObject = (CGameObject*)task.Param0;
			LONGLONG LayerIdx = (LONGLONG)task.Param1;

			// 부모 오브젝트의 경우
			if (nullptr == pObject->GetParent())
			{
				pObject->DisconnectWithLayer();
				pObject->MLayerIdx = (int)LayerIdx;
				pObject->RegisterAsParent();
			}

			// 자식 오브젝트의 경우
			else
			{
				pObject->MLayerIdx = (int)LayerIdx;
			}

			pObject->LayerMoveDone();
			m_LevelChanged = true;
		}
			break;
		case TASK_TYPE::DELETE_OBJECT:
		{
			CGameObject* pObject = (CGameObject*)task.Param0;
			if (!pObject)
			{
				LOG_ERROR("[Task] No Object To Delete");
				assert(!"No Object To Delete");
			}
			LOG_INFO_F("[Task] {} Now Deleted", WStringToString(pObject->GetName()));
			delete pObject;
			m_LevelChanged = true;
		}
			break;
		case TASK_TYPE::SETACTIVE_OBJECT:
		{
			// 여기에 온 건 모두 비활성화임
			CGameObject* pObject = (CGameObject*)task.Param0;

			// 비활성화 예정 상태가 아니라면 처리하지 않음. (한 프레임에 여러 번 세팅된 경우 대비)
			if (!pObject->IsDeactivated())
				break;

			pObject->SetActive(false);
			pObject->SetDeactivated(false);

			m_LevelChanged = true;
		}
			break;
		}
	}

	m_vecDelayedTask.clear();


	for (size_t i = 0; i < m_vecTask.size(); ++i)
	{
		tTask& task = m_vecTask[i];

		switch (task.Type)
		{
			// Delayed Task
		case TASK_TYPE::DELETE_OBJECT:
		{
			CGameObject* pObject = (CGameObject*)task.Param0;
			if (!pObject->IsDead())
			{
				queue<CGameObject*> Q;
				Q.emplace(pObject);

				// Dead 표시는 하위 hierarchy 전체에 해줌
				while (!Q.empty())
				{
					CGameObject* curObj = Q.front();
					Q.pop();

					curObj->SetDead(true);

					const vector<CGameObject*>& vecChild = curObj->GetChild();
					for (auto* child : vecChild)
					{
						Q.emplace(child);
					}
				}

				m_vecDelayedTask.push_back(std::move(task));
				LOG_INFO_F("[Task] {} Will Be Deleted", WStringToString(pObject->GetName()));
			}
		}
			break;
		case TASK_TYPE::ADD_CHILD:
		{
			CGameObject* pParent = (CGameObject*)task.Param0;
			CGameObject* pChild = (CGameObject*)task.Param1;

			// Layer 이동이 필요하다면
			if (pParent && pParent->GetLayerIdx() != pChild->GetLayerIdx())
				pChild->SetNextLayerIdx(pParent->GetLayerIdx());

			m_vecDelayedTask.push_back(std::move(task));
		}
			break;
		case TASK_TYPE::CHANGE_LAYEROBJECT:
		{
			CGameObject* pObject = (CGameObject*)task.Param0;
			LONGLONG LayerIdx = (LONGLONG)task.Param1;

			pObject->SetNextLayerIdx(static_cast<int>(LayerIdx));
			m_vecDelayedTask.push_back(std::move(task));
		}
			break;
		case TASK_TYPE::SETACTIVE_OBJECT:
		{
			CGameObject* pObject = (CGameObject*)task.Param0;
			bool bActive = (bool)task.Param1;

			// 한 프레임에서 여러 번 활성화 / 비활성화를 받은 경우 고려.
			// 마지막 세팅 값만을 남기도록 로직 구현.
			// m_Deactivate 변수를 계속 바꿔서 지연 task에서 분기처리

			// 활성화는 immediate
			if (bActive)
			{
				pObject->SetDeactivated(false);
				pObject->SetActive(true);
			}
			// 비활성화는 delay
			else
			{
				// 활성화 되어있고, 비활성화 예정도 아닌 경우에만 task 추가 (중복 제거)
				if (pObject->IsActive() && !pObject->IsDeactivated())
				{
					m_vecDelayedTask.push_back(std::move(task));
				}

				pObject->SetDeactivated(true);
			}
		}
			break;

			// Immediate Task
		case TASK_TYPE::CREATE_OBJECT:
			{
				auto pNewObject = (CGameObject*)task.Param0;
				int LayerIdx = static_cast<int>(task.Param1);

				CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
				pCurLevel->AddObject(LayerIdx, pNewObject, task.Param2);

				m_LevelChanged = true;

				if (pCurLevel->GetState() == LEVEL_STATE::PLAY
					|| pCurLevel->GetState() == LEVEL_STATE::PAUSE)
				{
					pNewObject->Begin();
				}
			}
			break;

		case TASK_TYPE::CHANGE_LEVEL:
			{
				auto pNextLevel = (CLevel*)task.Param0;

				auto eNextState = static_cast<LEVEL_STATE>(task.Param1);

				CLevelMgr::GetInst()->ChangeLevel(pNextLevel, eNextState);

				m_LevelChanged = true;
			}
			break;
		case TASK_TYPE::CHANGE_LEVEL_STATE:
			{
				auto eNextState = static_cast<LEVEL_STATE>(task.Param0);
				CLevelMgr::GetInst()->ChangeLevelState(eNextState);
			}
			break;

		case TASK_TYPE::DELETE_ASSET:
			{
				Ptr<CAsset> pAsset = (CAsset*)task.Param0;
				CAssetMgr::GetInst()->DeleteAsset(pAsset->GetAssetType(), pAsset->GetKey());
			}
			break;

		case TASK_TYPE::CHANGE_NAME:
			{
				CEntity* pEntity = (CEntity*)task.Param0;
				wstring* pName = (wstring*)task.Param1;
				pEntity->SetName(*pName);

				m_LevelChanged = true;
			}
			break;
		}
	}

	m_vecTask.clear();
}
