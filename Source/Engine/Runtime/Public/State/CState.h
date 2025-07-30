#pragma once
#include "Engine/Core/Public/CEntity.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

class CState :
	public CEntity
{
private:
	CStateMachine* m_Owner;
	ACTION_STATE m_OwnState;

public:
	CStateMachine* GetStateMachine() { return m_Owner; }
	ACTION_STATE GetOwnState() { return m_OwnState; }


public:
	virtual void Enter() = 0;
	virtual void FinalTick() = 0;
	virtual void Exit() = 0;

public:
	CState(ACTION_STATE _State);
	~CState();

	friend class CStateMachine;
};

