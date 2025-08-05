#pragma once
#include "Engine/Core/Public/CEntity.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

// State logic (특히 Finaltick)에서 state를 변경하는 경우 사용하는 매크로
// 상태 변경 후 바로 return 함으로써 뒤 로직은 실행하지 않게 하기 위해 작성함.
#define CHANGE_STATE_AND_RETURN(state) if (GetStateMachine()->ChangeState(static_cast<int>(state)))	\
									   {															\
									   		return;													\
									   }

// State를 애셋 개념이 아니라 StateMachine에 종속된 자원으로 생각
class CState
	: public CEntity
{
	friend class CStateMachine;

protected:
	wstring				m_ClipName;		// 실행할 애니메이션 클립이름
	float				m_Delay;		// 애니메이션 딜레이

	set<wstring>		m_Transitions;	// 이 상태로부터 전환이 가능한 상태 목록

private:
	CStateMachine* m_Owner;

public:
	CStateMachine* SM() const { return m_Owner; }
	void SetOwner(CStateMachine* _StateMachine) { m_Owner = _StateMachine; }

	void AdjustAnim();

	virtual void Enter() = 0;
	virtual void FinalTick() = 0;
	virtual void Exit() = 0;

	virtual void Save(FILE* _File);
	virtual void Load(FILE* _File);

private:
	bool AddTransition(const wstring& _StateName);
	bool DeleteTransition(const wstring& _StateName);
	bool IsTransitionable(const wstring& _StateName) { return m_Transitions.contains(_StateName); }

public:
	virtual CState* Clone() = 0;
	CState(const wstring& _Name);
	~CState();
};
