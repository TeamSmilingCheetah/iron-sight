#include "pch.h"
#include "System/Public/Manager/CTaskMgr.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "Runtime/Public/Actor/CLevel.h"
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

	for (size_t i = 0; i < m_vecGC.size(); ++i)
	{
		delete m_vecGC[i];
		m_LevelChanged = true;
	}
	m_vecGC.clear();


	for (size_t i = 0; i < m_vecTask.size(); ++i)
	{
		const tTask& task = m_vecTask[i];

		switch (task.Type)
		{
		case TASK_TYPE::ADD_CHILD:
			{
				auto pParent = (CGameObject*)task.Param0;
				auto pChild = (CGameObject*)task.Param1;

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
						pChild->m_LayerIdx = LayerIdx;
					}

					pParent->AddChild(pChild);
				}

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
					pObject->m_LayerIdx = (int)LayerIdx;
					pObject->RegisterAsParent();
				}

				// 자식 오브젝트의 경우
				else
				{
					pObject->m_LayerIdx = (int)LayerIdx;
				}

				m_LevelChanged = true;
			}
			break;
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
		case TASK_TYPE::DELETE_OBJECT:
			{
				auto pObject = (CGameObject*)task.Param0;
				if (!pObject->IsDead())
				{
					pObject->m_Dead = true;
					m_vecGC.push_back(pObject);
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
		}
	}

	m_vecTask.clear();
}
