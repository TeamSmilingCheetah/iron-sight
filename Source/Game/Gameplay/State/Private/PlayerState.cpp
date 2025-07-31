#include "pch.h"
#include "Game/Gameplay/State/Public/PlayerState.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"

PlayerState::PlayerState(const wstring& _Name)
	: CState(_Name)
{
}

PlayerState::~PlayerState()
{
}

void PlayerState::Enter()
{
	if (!m_PlayerScript)
	{
		m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(GetStateMachine()->GetOwner(), SCRIPT_TYPE::PLAYERSCRIPT));
	}

	// 애니메이션 재생이 끝나야 상태 전환 가능한 애니메이션인 경우
	if (!m_CanExitDuringAnimation)
	{
		// 상태 전환 불가능하도록 설정
		GetStateMachine()->SetCanExit(false);
	}

	Enter_Override();
}

void PlayerState::FinalTick()
{
	// TODO: 매 프레임 하지 않도록 개선?
	CStateMachine* pStateMachine = GetStateMachine();

	// 애니메이션이 종료되어야 전환 가능한 상태인 경우
	if (!pStateMachine->CanExit() && !m_CanExitDuringAnimation)
	{
		// 애니메이터가 멈춘 경우 전환 가능 상태로 변경
		if (!pStateMachine->Animator3D()->IsActive())
		{
			pStateMachine->SetCanExit(true);
		}
	}

	// TODO: CState 쪽으로 올리기
	// 다음 State가 지정되어 있고, 상태 전환 가능한 경우
	//if (GetNextStateIndex() != -1 && pStateMachine->CanExit())
	//{
	//	// 바로 상태 전환 호출
	//	pStateMachine->ChangeState(GetNextStateIndex());
	//}

	FinalTick_Override();
}
