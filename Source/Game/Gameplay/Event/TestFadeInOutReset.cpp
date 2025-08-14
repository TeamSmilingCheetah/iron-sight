#include "pch.h"
#include "Game/Gameplay/Event/TestFadeInOutReset.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"

#include "Game/Gameplay/Character/Public/CameraEffect.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

#include "Engine/Runtime/Public/Component/Base/components.h"

TestFadeInOutReset::TestFadeInOutReset()
	:EventScriptBase(SCRIPT_TYPE::TESTFADEINOUTRESET)
{
	// 이벤트 지속시간 2초로 설정
	m_EventDuration = 2.f;
}

TestFadeInOutReset::~TestFadeInOutReset()
{
}

void TestFadeInOutReset::OnInitialize()
{
	// 플레이어, 카메라효과스크립트 등록
	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");

	CGameObject* CameraPost = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"CameraPost");
	m_CameraEffect = static_cast<CameraEffect*>(GetScriptWithType(CameraPost, SCRIPT_TYPE::CAMERAEFFECT));
}

void TestFadeInOutReset::OnEvent()
{
	// 이벤트 도중에는 하는거 없음
}

void TestFadeInOutReset::OnEventStart()
{
	// 페이드 아웃
	m_CameraEffect->FadeOut();
}

void TestFadeInOutReset::OnEventEnd()
{
	// 페이드 인
	m_CameraEffect->FadeIn();

	// 플레이어 위치 변경
	m_Player->Transform()->SetRelativePos(4000.f, 500.f, 1500.f);

	// 재 호출이 가능하도록 리셋
	ForceReset();

}

bool TestFadeInOutReset::CheckEventStart()
{
	// 이벤트 시작 : 키 누름
	if (KEY_TAP(KEY::PLUS))
	{
		return true;
	}

	return false;
}

bool TestFadeInOutReset::CheckEndEvent()
{
	// 이벤트 종료 : 설정된 이벤트 시간 종료 시

	return m_Eventtime <= 0.0f;
}
