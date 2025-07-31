#include "pch.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/Runtime/Public/State/CState.h"

CStateMachine::CStateMachine()
	: CComponent(COMPONENT_TYPE::STATEMACINE)
	, m_CurState(nullptr)
	, m_CanExit(true)
{
}

CStateMachine::~CStateMachine()
{
	DeleteMap(m_mapState);
}

void CStateMachine::AddState(CState* _State)
{
	// map에 추가
	m_mapState.emplace(_State->GetName(), _State);

	// state에서도 statemachine에 접근할 수 있도록 알려줌
	_State->SetOwner(this);
}

bool CStateMachine::ChangeState(const wstring& _Name)
{
	// 현재 상태가 exit 할 수 없는 상황이라면 바로 return
	if (CanExit())
	{
		return false;
	}

	auto iter = m_mapState.find(_Name);

	if (iter == m_mapState.end())
	{
		return false;
	}

	m_PrevState = m_CurState;
	m_CurState = iter->second;

	// 이전 state는 exit
	if (m_PrevState)
	{
		m_PrevState->Exit();
	}

	// 현재 state에 enter
	m_CurState->Enter();

	return true;
}

void CStateMachine::FinalTick()
{
	if (m_CurState)
		m_CurState->FinalTick();
}

void CStateMachine::SaveComponent(FILE* _File)
{
	//int vecSize = static_cast<int>(m_vecState.size());
	//fwrite(&vecSize, sizeof(int), 1, _File);
	//
	//for (int i = 0; i < vecSize; ++i)
	//{
	//	CState->Save();
	//}
}

void CStateMachine::LoadComponent(FILE* _File)
{
}
