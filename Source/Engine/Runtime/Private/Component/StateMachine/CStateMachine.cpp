#include "pch.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/Runtime/Public/State/CState.h"
#include "Engine/System/Public/Manager/CStateMgr.h"

CStateMachine::CStateMachine()
	: FComponent(COMPONENT_TYPE::STATEMACINE)
	, m_DefaultState(nullptr)
	, m_PrevState(nullptr)
	, m_CurState(nullptr)
	, m_NextState(nullptr)
	, m_CanExit(true)
	, m_StateChanged(false)
{
}

CStateMachine::~CStateMachine()
{
	DeleteMap(m_mapState);
}

void CStateMachine::AddState(CState* _State)
{
	assert(_State != nullptr);

	// map에 추가
	m_mapState.emplace(_State->GetName(), _State);

	// state에서도 statemachine에 접근할 수 있도록 알려줌
	_State->SetOwner(this);

	// 만약 default state가 없다면 (최초 추가) default state로 설정
	if (m_DefaultState == nullptr)
	{
		m_DefaultState = _State;
	}
}

void CStateMachine::SetDefaultState(const wstring& _Name)
{
	m_DefaultState = FindState(_Name);
}

CState* CStateMachine::FindState(const wstring& _Name)
{
	auto iter = m_mapState.find(_Name);

	if (iter == m_mapState.end())
		return nullptr;

	return iter->second;
}

void CStateMachine::SetChange(const wstring& _Name)
{
	// 현재 상태가 exit 할 수 없는 상황이라면 바로 return
	if (!m_CanExit)
	{
		return;
	}

	CState* pState = FindState(_Name);

	// 바꾸려는 상태가 존재하지 않는 state라면 종료
	if (pState == nullptr)
	{
		return;
	}

	// 현재 상태로부터 전이 가능하지 않은 상태라면 바로 리턴
	// 1. Any State의 Transitions
	// 2. 현재 State의 Transitions
	if ((m_CurState && !m_CurState->IsTransitionable(_Name))
		&& !m_AnyTransitions.contains(_Name))
	{
		return;
	}

	m_NextState = pState;
	m_StateChanged = true;
}

void CStateMachine::ChangeState(CState* _State)
{
	m_PrevState = m_CurState;
	m_CurState = _State;

	// 이전 state exit
	if (m_PrevState)
		m_PrevState->Exit();

	// 현재 state에 enter
	m_CurState->Enter();
}

bool CStateMachine::AddAnyTransition(const wstring& _To)
{
	// StateMachine에 존재하는 경우에만 추가
	auto iter = m_mapState.find(_To);
	if (iter == m_mapState.end())
	{
		assert(!"Animation doesn't exist");
		return false;
	}

	// Any State Transition 추가
	m_AnyTransitions.emplace(_To);
	return true;
}

bool CStateMachine::DeleteAnyTransition(const wstring& _To)
{
	auto iter = m_AnyTransitions.find(_To);
	if (iter == m_AnyTransitions.end())
	{
		assert(!"Animation doesn't exist");
		return false;
	}

	m_AnyTransitions.erase(iter);
	return true;
}

bool CStateMachine::AddTransition(const wstring& _From, const wstring& _To)
{
	// StateMachine에 존재하는 경우에만 추가
	auto it1 = m_mapState.find(_From);
	if (it1 == m_mapState.end())
	{
		assert(!"Animation doesn't exist");
		return false;
	}

	auto it2 = m_mapState.find(_To);
	if (it2 == m_mapState.end())
	{
		assert(!"Animation doesn't exist");
		return false;
	}

	// Before State에 Transition 추가
	return it1->second->AddTransition(_To);
}

bool CStateMachine::DeleteTransition(const wstring& _From, const wstring& _To)
{
	auto it = m_mapState.find(_From);
	if (it == m_mapState.end())
	{
		assert(!"Animation doesn't exist");
		return false;
	}

	// Befor State에서 Transition 제거
	return it->second->DeleteTransition(_To);
}

void CStateMachine::Begin()
{
	// Default state로 진입 (entry)
	ChangeState(m_DefaultState);
}

void CStateMachine::FinalTick()
{
	// Script에서 State 변경 요청이 들어왔다.
	if (m_StateChanged)
	{
		ChangeState(m_NextState);
		m_StateChanged = false;
	}

	if (m_CurState)
		m_CurState->FinalTick();
}

void CStateMachine::SaveComponent(FILE* _File)
{
	// default state 이름 저장
	assert(m_DefaultState != nullptr);
	SaveWString(m_DefaultState->GetName(), _File);

	// state 목록 저장
	int stateCount = static_cast<int>(m_mapState.size());
	fwrite(&stateCount, sizeof(int), 1, _File);
	
	for (auto iter = m_mapState.begin(); iter != m_mapState.end(); ++iter)
	{
		SaveWString(iter->first, _File);
		iter->second->Save(_File);
	}

	// anyTransitions 저장
	int transitionCount = static_cast<int>(m_AnyTransitions.size());
	fwrite(&transitionCount, sizeof(int), 1, _File);
	
	for (auto iter = m_AnyTransitions.begin(); iter != m_AnyTransitions.end(); ++iter)
	{
		SaveWString(*iter, _File);
	}
}

void CStateMachine::LoadComponent(FILE* _File)
{
	wstring defaultName;

	// default state 이름 로드
	LoadWString(defaultName, _File);

	// state 목록 로드
	int stateCount;
	fread(&stateCount, sizeof(int), 1, _File);

	wstring stateName;
	CState* pState;
	for (int i = 0; i < stateCount; ++i)
	{
		LoadWString(stateName, _File);

		// StateMgr에서 derived 객체를 받아옴
		pState = CStateMgr::GetInst()->GetState(stateName);
		pState->Load(_File);

		// default state 찾기
		if (pState->GetName() == defaultName)
		{
			m_DefaultState = pState;
		}

		AddState(pState);
	}

	
	
	// anyTransitions 로드
	int transitionCount;
	fread(&transitionCount, sizeof(int), 1, _File);
	
	for (int i = 0; i < transitionCount; ++i)
	{
		LoadWString(stateName, _File);
		m_AnyTransitions.emplace(stateName);
	}
}


