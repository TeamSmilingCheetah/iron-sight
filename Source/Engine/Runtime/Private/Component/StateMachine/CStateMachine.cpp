#include "pch.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

#include "Engine/Runtime/Public/State/CState.h"

CStateMachine::CStateMachine()
	: CComponent(COMPONENT_TYPE::STATEMACINE)
	, m_CurMotionState(MOTION_STATE::STAND)
	, m_CurActionState(nullptr)
	, m_PrevMotionState(MOTION_STATE::STAND)
	, m_PrevActionState(nullptr)
{
}

CStateMachine::CStateMachine(const CStateMachine& _Other)
	: CComponent(COMPONENT_TYPE::STATEMACINE)
{
}

CStateMachine::~CStateMachine()
{
	DeleteMap(m_mapActionState);

	//delete m_CurActionState;
	//delete m_PrevActionState;
}


void CStateMachine::ChangeActionState(ACTION_STATE _State)
{
	CState* pNextState = FindState(_State);

	m_PrevActionState = m_CurActionState;

	// 기존 상태를 빠져나온다. (Exit)
	if (nullptr != m_CurActionState)
	{
		m_CurActionState->Exit();
	}

	// 새로운 상태로 현재 상태를 바꿔준다.
	m_CurActionState = pNextState;
	assert(m_CurActionState);

	// 새로운 상태로 진입한다. (Enter)
	m_CurActionState->Enter();
}


void CStateMachine::AddState(CState* _State)
{
	m_mapActionState.insert({ (UINT)_State->GetOwnState(), _State });
	_State->m_Owner = this;
}

CState* CStateMachine::FindState(ACTION_STATE _State)
{
	map<UINT, CState*>::iterator iter = m_mapActionState.find((UINT)_State);
	if (iter == m_mapActionState.end())
	{
		return nullptr;
	}

	return iter->second;
}

void CStateMachine::FinalTick()
{
	if (nullptr == m_CurActionState)
		return;

	m_CurActionState->FinalTick();
}

void CStateMachine::SaveComponent(FILE* _File)
{
}

void CStateMachine::LoadComponent(FILE* _File)
{
}
