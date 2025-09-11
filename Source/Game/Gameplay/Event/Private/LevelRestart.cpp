#include "pch.h"

#include "Game/Gameplay/Event/Public/LevelRestart.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/GamePlay/Inventory/Public/InventoryController.h"
#include "Game/System/Public/CGameMgr.h"

LevelRestart::LevelRestart()
	: EventScriptBase(SCRIPT_TYPE::LEVELRESTART)
	, m_vecSanpshot()
	, m_InventoryScript(nullptr)
	, m_PlayerScript(nullptr)
{
	m_EventDuration = 2.5f;
}

LevelRestart::~LevelRestart()
{
	for (auto& p : m_vecSanpshot)
	{
		DELETE(p.second);
	}
	m_vecSanpshot.clear();
}


void LevelRestart::OnInitialize()
{
	// Script
	m_InventoryScript = CGameMgr::GetInst()->GetInventoryScript();
	m_PlayerScript = CGameMgr::GetInst()->GetPlayerScript();


	// Level Start SnapShot
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	// items
	vector<CGameObject*> vecObjs = pCurLevel->GetLayer(6)->GetParentObjects();
	for (int i = 0; i < vecObjs.size(); ++i)
	{
		m_vecSanpshot.push_back(pair<int, CGameObject*>(vecObjs[i]->GetLayerIdx(), vecObjs[i]->Clone()));
	}

	// Enemy
	vecObjs = pCurLevel->GetLayer(7)->GetParentObjects();
	for (int i = 0; i < vecObjs.size(); ++i)
	{
		m_vecSanpshot.push_back(pair<int, CGameObject*>(vecObjs[i]->GetLayerIdx(), vecObjs[i]->Clone()));
	}
}


void LevelRestart::OnEvent()
{

}

void LevelRestart::OnEventStart()
{
	// Level에 남아있는 Item과 Enemy오브젝트 전부 삭제
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	vector<CGameObject*> vecObjs = pCurLevel->GetLayer(6)->GetParentObjects();
	for (int i = 0; i < vecObjs.size(); ++i)
	{
		DestroyObject(vecObjs[i]);
	}

	vecObjs = pCurLevel->GetLayer(7)->GetParentObjects();
	for (int i = 0; i < vecObjs.size(); ++i)
	{
		DestroyObject(vecObjs[i]);
	}

	// 초기에 저장해놓은 Snapshot을 레벨에 추가
	for (int i = 0; i < m_vecSanpshot.size(); ++i)
	{
		pCurLevel->AddObject(m_vecSanpshot[i].first, m_vecSanpshot[i].second, false);
	}

	// Player Inventory 초기화
	m_InventoryScript->ClearInventory();
}

void LevelRestart::OnEventEnd()
{
	// SnapShot 벡터 Clear 및 재저장
	m_vecSanpshot.clear();

	// Level Start SnapShot
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	// items
	vector<CGameObject*> vecObjs = pCurLevel->GetLayer(6)->GetParentObjects();
	for (int i = 0; i < vecObjs.size(); ++i)
	{
		m_vecSanpshot.push_back(pair<int, CGameObject*>(vecObjs[i]->GetLayerIdx(), vecObjs[i]->Clone()));
	}

	// Enemy
	vecObjs = pCurLevel->GetLayer(7)->GetParentObjects();
	for (int i = 0; i < vecObjs.size(); ++i)
	{
		m_vecSanpshot.push_back(pair<int, CGameObject*>(vecObjs[i]->GetLayerIdx(), vecObjs[i]->Clone()));
	}

	// 
	m_PlayerScript->SetGameResetting(false);

	// 재 호출이 가능하도록 리셋
	ForceReset();
}

bool LevelRestart::CheckEventStart()
{
	return m_bCanEvent;
}

bool LevelRestart::CheckEndEvent()
{
	return m_Eventtime <= 0.0f;
}

