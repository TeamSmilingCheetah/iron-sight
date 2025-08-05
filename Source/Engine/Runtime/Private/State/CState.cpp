#include "pch.h"
#include "Engine/Runtime/Public/State/CState.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"



CState::CState(const wstring& _Name)
	: m_Owner(nullptr)
	, m_Delay(0.f)
{
	SetName(_Name);
}

CState::~CState()
{
}


void CState::AdjustAnim()
{
	if (m_Owner->GetOwner()->Animator3D()->GetCurClip()->GetKey() != m_ClipName
		&& (m_Owner->GetOwner()->Animator3D()->GetNextClip() == nullptr || m_Owner->GetOwner()->Animator3D()->GetNextClip()->GetKey() != m_ClipName))
	{
		m_Owner->GetOwner()->Animator3D()->SetCurClipBlend(m_ClipName, m_Delay);
	}
}

void CState::Save(FILE* _File)
{
	fwrite(&m_Delay, sizeof(float), 1, _File);
	
	int stateCount = static_cast<int>(m_Transitions.size());
	fwrite(&stateCount, sizeof(int), 1, _File);
	
	for (auto iter = m_Transitions.begin(); iter != m_Transitions.end(); ++iter)
	{
		SaveWString(*iter, _File);
	}
}

void CState::Load(FILE* _File)
{
	fread(&m_Delay, sizeof(float), 1, _File);
	
	int stateCount;
	fread(&stateCount, sizeof(int), 1, _File);
	
	wstring stateName;
	for (int i = 0; i < stateCount; ++i)
	{
		LoadWString(stateName, _File);
		m_Transitions.emplace(stateName);
	}
}

bool CState::AddTransition(const wstring& _StateName)
{
	if (m_Transitions.contains(_StateName))
	{
		return false;
	}

	m_Transitions.emplace(_StateName);

	return true;
}

bool CState::DeleteTransition(const wstring& _StateName)
{
	auto iter = m_Transitions.find(_StateName);

	if (iter == m_Transitions.end())
	{
		return false;
	}

	m_Transitions.erase(iter);

	return true;
}
