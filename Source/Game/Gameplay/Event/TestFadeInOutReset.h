#pragma once
#include "Game/Gameplay/Event/EventScriptBase.h"

class CameraEffect;

class TestFadeInOutReset :
    public EventScriptBase
{
private:
	CGameObject* m_Player;
	CameraEffect* m_CameraEffect;

private:
	// 자식 클래스의 시작 시 호출되는 함수
	// Begin()에서 자동으로 호출됨
	virtual void OnInitialize() override;

	//이벤트 발동 조건을 체크하는 함수
	//true면 이벤트 시작, false면 대기 상태 유지
	virtual bool CheckEventStart() override;

	// 이벤트가 진행 중일 때 매 프레임 호출되는 함수
	// 각 이벤트의 고유한 로직을 여기에 구현
	virtual void OnEvent() override;

	// 이벤트 시작 시 호출되는 초기화 함수
	// CheckEventCondition()이 true를 반환했을 때 한 번 호출됨
	virtual void OnEventStart() override;

	// 이벤트 종료 시 호출되는 정리 함수
	// 시간 종료나 강제 종료 시 한 번 호출됨
	virtual void OnEventEnd() override;

	// 이벤트 종료 조건을 체크하는 함수
	// true면 이벤트 종료
	virtual bool CheckEndEvent() override;

public:
	virtual void SaveComponent(FILE* _File) override {}
	virtual void LoadComponent(FILE* _File) override {}

	void SetEventStart() { m_bCanEvent = true; }

	TestFadeInOutReset();
	virtual ~TestFadeInOutReset();
	CLONE(TestFadeInOutReset)
};

