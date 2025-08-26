#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"


class EventScriptBase :
    public CScript
{
protected:
	bool                 m_EventEffect;      // 이벤트 진행 중인지 여부
	bool                 m_EndEvent;         // 이벤트 완전 종료 여부
	bool                 m_EventReset;		 // 이벤트 리셋 명령 확인 여부

	bool				 m_bCanEvent;		 // 임시

	float                m_EventDuration;    // 이벤트 지속 시간
	float                m_Eventtime;        // 현재 남은 시간

public:

	// 해당 가상함수를 자식에서 재선언하는걸 막지는 않음
	// 어지간하면 아래 함수들 사용 권장
	virtual void Begin() override;
	virtual void Tick() override;

private:
	// 자식 클래스에서 구현해야 하는 가상 함수들

	// 자식 클래스의 시작 시 호출되는 함수
	// Begin()에서 자동으로 호출됨
	virtual void OnInitialize() {}

	//이벤트 발동 조건을 체크하는 함수
	//true면 이벤트 시작, false면 대기 상태 유지
	virtual bool CheckEventStart() { return false; };

	// 이벤트가 진행 중일 때 매 프레임 호출되는 함수
	// 각 이벤트의 고유한 로직을 여기에 구현
	virtual void OnEvent() {};

	// 이벤트 시작 시 호출되는 초기화 함수
	// CheckEventCondition()이 true를 반환했을 때 한 번 호출됨
	virtual void OnEventStart() {}

	// 이벤트 종료 시 호출되는 정리 함수
	// 시간 종료나 강제 종료 시 한 번 호출됨
	virtual void OnEventEnd() {}

	// 이벤트 종료 조건을 체크하는 함수
	// true면 이벤트 종료
	virtual bool CheckEndEvent();

	// 이벤트가 재사용 되도록 리셋하는 함수
	virtual void ResetEvent() {};

private:
	// 이벤트를 시작하는 함수
	void StartEvent();

	// 이벤트를 종료하는 함수
	void EndEvent();

	// 이벤트 시간을 업데이트하는 함수
	void UpdateEventTime();

public:
	bool IsEventEffect() const { return m_EventEffect; }
	bool IsEventEnded() const { return m_EndEvent; }

	void SetEventDuration(float _Duration) { m_EventDuration = _Duration; }
	void SetEventStart() { m_bCanEvent = true; }

	float GetEventDuration() const { return m_EventDuration; }
	float GetRemainingTime() const { return m_Eventtime; }

	//이벤트를 강제로 종료하는 함수
	void ForceEndEvent() { m_EndEvent = true; EndEvent(); }

	// 이벤트 리셋용
	void ForceReset();

public:
	virtual void SaveComponent(FILE* _File) override {}
	virtual void LoadComponent(FILE* _File) override {}


	EventScriptBase(SCRIPT_TYPE _ScriptType);
	virtual ~EventScriptBase();
	CLONE(EventScriptBase)
};

