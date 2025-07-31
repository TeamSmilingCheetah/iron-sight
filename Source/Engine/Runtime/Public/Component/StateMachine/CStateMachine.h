#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

class CState;

class CStateMachine
	: public CComponent
{
private:
	map<wstring, CState*>		m_mapState;

	CState*				m_PrevState;
	CState*				m_CurState;
	bool				m_CanExit;	// 다른 state로 변경 가능한 상태인지

public:
	CState* GetState(const wstring& _Key) const { return m_mapState.find(_Key)->second; }
	void AddState(CState* _State);

	void SetCanExit(bool _b) { m_CanExit = _b; }
	bool CanExit() const { return m_CanExit; }

	CState* GetPrevState() const { return m_PrevState; }
	CState* GetCurState() const { return m_CurState; }

	map<wstring, CState*> GetStateMap() { return m_mapState; }

	bool ChangeState(const wstring& _Name);	// State 구현부에서 호출, state 바뀌었는지 여부 리턴

	virtual void FinalTick() override;
	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

public:
	CLONE(CStateMachine)
	CStateMachine();
	~CStateMachine();
};
