#include "pch.h"

#include "Game/Gameplay/Event/Public/PlayerRevive.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Base/components.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/System/Public/CGameMgr.h"
#include "Game/Gameplay/Character/Public/CameraEffect.h"

PlayerRevive::PlayerRevive()
	: EventScriptBase(SCRIPT_TYPE::PLAYERREVIVE)
	, m_Player(nullptr)
	, m_PlayerScript(nullptr)
{
	m_EventDuration = 2.5f;
}

PlayerRevive::~PlayerRevive()
{

}


void PlayerRevive::OnInitialize()
{
	// 플레이어, 카메라효과스크립트 등록
	m_Player = CGameMgr::GetInst()->GetPlayer();
	m_PlayerScript = CGameMgr::GetInst()->GetPlayerScript();
	m_CameraEffect = CGameMgr::GetInst()->GetCamEffect();
}



void PlayerRevive::OnEvent()
{

}

void PlayerRevive::OnEventStart()
{
	// 페이드 인
	m_bCanEvent = false;
	m_CameraEffect->FadeIn();

	// 플레이어 위치 변경
	m_Player->Transform()->SetRelativePos(4000.f, 500.f, 1500.f);
	m_PlayerScript->SetRevive();
}

void PlayerRevive::OnEventEnd()
{
	// Fade 효과 없애기
	m_CameraEffect->FadeInOutOff();

	// 재 호출이 가능하도록 리셋
	ForceReset();
}

bool PlayerRevive::CheckEventStart()
{
	return m_bCanEvent;
}

bool PlayerRevive::CheckEndEvent()
{
	return m_Eventtime <= 0.0f;
}

