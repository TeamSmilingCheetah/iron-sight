#include "pch.h"
#include "Game/GameStart.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/System/Public/CGameMgr.h"
#include "Game/Gameplay/Character/Public/CameraEffect.h"


GameStart::GameStart()
	: EventScriptBase(SCRIPT_TYPE::GAMESTART)
{
	m_EventDuration = 0.1f;
}

GameStart::~GameStart()
{
}

void GameStart::OnInitialize()
{
	m_CameraEffect = CGameMgr::GetInst()->GetCamEffect();
	m_PlayerScript = CGameMgr::GetInst()->GetPlayerScript();
}

bool GameStart::CheckEventStart()
{
	return m_bCanEvent;
}

void GameStart::OnEvent()
{
}

void GameStart::OnEventStart()
{
	// 페이드 인
	m_CameraEffect->FadeIn();
	// 마우스 잠금
	m_PlayerScript->SetMouseActive(false);
}

void GameStart::OnEventEnd()
{
}

bool GameStart::CheckEndEvent()
{
	return m_Eventtime <= 0.f;
}
