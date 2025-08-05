#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

class CState;

class CStateMachine
	: public CComponent
{
private:
	map<wstring, CState*>		m_mapState;

	CState*				m_DefaultState;	// 기본 state
	CState*				m_PrevState;
	CState*				m_CurState;
	CState*				m_NextState;

	bool				m_CanExit;	// 다른 state로 변경 가능한 상태인지
	bool				m_StateChanged; // 저번 Tick동안 State변경 요청이 있었는지

	set<wstring>		m_AnyTransitions;	// 모든 State에서 전이 가능한 State들 목록

public:
	CState* GetState(const wstring& _Name) const { return m_mapState.find(_Name)->second; }
	void AddState(CState* _State);
	void SetDefaultState(const wstring& _Name);

	CState* FindState(const wstring& _Name);

	void SetCanExit(bool _b) { m_CanExit = _b; }
	void SetChange(const wstring& _Name);

	bool CanExit() const { return m_CanExit; }

	CState* GetPrevState() const { return m_PrevState; }
	CState* GetCurState() const { return m_CurState; }

	const map<wstring, CState*>& GetStateMap() { return m_mapState; }

	// Transition
	bool AddAnyTransition(const wstring& _To);
	bool DeleteAnyTransition(const wstring& _To);
	bool AddTransition(const wstring& _From, const wstring& _To);
	bool DeleteTransition(const wstring& _From, const wstring& _To);

	virtual void Begin() override;
	virtual void FinalTick() override;
	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

private:
	void ChangeState(CState* _State);	// State 구현부에서 호출, state 바뀌었는지 여부 리턴

public:
	CLONE(CStateMachine)
	CStateMachine();
	~CStateMachine();
};
