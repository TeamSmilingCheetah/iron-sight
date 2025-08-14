#include "pch.h"
#include "Game/Gameplay/Event/EventScriptBase.h"

#include "Engine/System/Public/Manager/CTimeMgr.h"


EventScriptBase::EventScriptBase(SCRIPT_TYPE _ScriptType)
	: CScript(_ScriptType)
	, m_EventEffect(false)
	, m_EndEvent(false)
	, m_EventReset(false)
	, m_EventDuration(3.0f)
	, m_Eventtime(0.0f)
{
}

EventScriptBase::~EventScriptBase()
{
}

void EventScriptBase::Begin()
{
	// 자식 클래스의 초기화 로직 호출
	// 불필요하더라도 Begin과 Tick함수 사용을 자제하기 위해 따로 함수 생성
	OnInitialize();
}

void EventScriptBase::Tick()
{
	// 이벤트가 완전히 종료된 경우 더 이상 처리하지 않음
	if (m_EndEvent)
	{
		// 리셋 명령이 있으면 리셋 처리
		if (m_EventReset)
		{
			ResetEvent();
			m_EndEvent = false;
			m_EventEffect = false;
			m_EventReset = false;
		}
		return;
	}

	// 이벤트 진행 중인지 확인
	if (m_EventEffect)
	{
		// 이벤트 진행 중
		// 각 자식의 고유 로직 실행
		OnEvent();

		// 공통 시간 업데이트
		UpdateEventTime();

		// 종료 조건 체크
		if (CheckEndEvent())
		{
			EndEvent();
		}
	}
	// 이벤트 대기 중
	else
	{
		// 발동 조건 체크
		if (CheckEventStart())
		{
			StartEvent();
		}
	}
}


void EventScriptBase::StartEvent()
{
	// 이벤트 상태 변경
	m_EventEffect = true;

	// 이벤트 시간 초기화
	m_Eventtime = m_EventDuration;

	// 자식 클래스의 추가 시작 로직
	OnEventStart();
}

void EventScriptBase::EndEvent()
{
	// 이벤트 완전 종료 플래그 설정
	m_EndEvent = true;

	// 자식 클래스의 추가 종료 로직
	OnEventEnd();
}

void EventScriptBase::ForceReset()
{
	ResetEvent();

	// 리셋해야한다는 명령을 기억함
	m_EventReset = true;
}

void EventScriptBase::UpdateEventTime()
{
	// 기본적으로는 매 틱마다 이벤트 시간 감소
	m_Eventtime -= DT;
}

bool EventScriptBase::CheckEndEvent()
{
	// 기본적으로는 시간이 다 되면 종료
	// 조건을 변경을 원하면은 자식 클래스에서 작성
	return m_Eventtime <= 0.0f;
}
