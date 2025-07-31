#pragma once
#include "Engine/Core/Public/CEntity.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

// State logic (특히 Finaltick)에서 state를 변경하는 경우 사용하는 매크로
// 상태 변경 후 바로 return 함으로써 뒤 로직은 실행하지 않게 하기 위해 작성함.
#define CHANGE_STATE_AND_RETURN(state) if (GetStateMachine()->ChangeState(static_cast<int>(state)))	\
									   {															\
									   		return;													\
									   }

class CState
	: public CEntity
{
	friend class CStateMachine;

private:
	CStateMachine*		m_Owner;

public:
	CStateMachine* GetStateMachine() const { return m_Owner; }
	void SetOwner(CStateMachine* _StateMachine) { m_Owner = _StateMachine; }

	virtual void Enter() = 0;
	virtual void FinalTick() = 0;
	virtual void Exit() = 0;

public:
	virtual CState* Clone() = 0;
	CState(const wstring& _Name);
	~CState();
};
