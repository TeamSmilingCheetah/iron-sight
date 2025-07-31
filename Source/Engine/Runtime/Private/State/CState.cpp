#include "pch.h"
#include "Engine/Runtime/Public/State/CState.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

CState::CState(const wstring& _Name)
	: m_Owner(nullptr)
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

