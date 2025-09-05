#include "pch.h"
#include "Game/Gameplay/State/Public/PlayerState.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Game/System/Public/CGameMgr.h"
#include "Game/Gameplay/Character/Public/CameraController.h"

PlayerState::PlayerState(const wstring& _Name)
	: CState(_Name)
	, m_CanExitDuringAnimation(true)
	, m_PlayerIdle(false)
{
}

PlayerState::~PlayerState()
{
}

void PlayerState::Enter()
{
	// TODO(Ssio): State에 Script를 저장하는 게 좀.. 맞나 싶네
	if (!m_PlayerScript)
	{
		m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(SM()->GetOwner(), SCRIPT_TYPE::PLAYERSCRIPT));
	}

	// 애니메이션 재생이 끝나야 상태 전환 가능한 애니메이션인 경우
	if (!m_CanExitDuringAnimation)
	{
		// 상태 전환 불가능하도록 설정
		SM()->SetCanExit(false);
	}

	// Override Function
	Enter_Override();
}

void PlayerState::FinalTick()
{
	// 애니메이션이 종료되어야 전환 가능한 상태인 경우
	if (!SM()->CanExit() && !m_CanExitDuringAnimation)
	{
		// m_CanExitDuringAnimation이 false라면 loop 하면 안됨.
		//assert(!SM()->Animator3D()->GetCurClip()->IsLoop());

		// 애니메이터가 멈춘 경우 전환 가능 상태로 변경
		if (!SM()->Animator3D()->IsActive())
		{
			SM()->SetCanExit(true);
		}
	}

	// Player의 움직임 조작 & DeadState 에선 무시
	if (m_PlayerScript->GetStateEnum() != PLAYER_STATE::Player_Dead)
	{
		m_PlayerScript->ProgressPlayerMove();
	}

	// PlayerState는 매 틱 입력을 감지해 움직이는 애니메이션을 적용해줘야 한다.
	ControlMoveAnimation();

	// Override Function
	FinalTick_Override();

	if (!SM()->CanExit() && !m_CanExitDuringAnimation)
	{
		// m_CanExitDuringAnimation이 false라면 loop 하면 안됨.
		//assert(!SM()->Animator3D()->GetCurClip()->IsLoop());

		// 애니메이터가 멈춘 경우 전환 가능 상태로 변경
		if (!SM()->Animator3D()->IsActive())
		{
			SM()->SetCanExit(true);
		}
	}
}

void PlayerState::Exit()
{
	// State 내부에서 동일하게 사용되는 AccTime 초기화
	m_PlayerScript->ResetAccTime();

	// Override Function
	Exit_Override();
}


void PlayerState::ControlMoveAnimation()
{
	// 상체 하체의 애니메이션이 따로 적용되지 않고 있기 때문에 움직이는 애니메이션을 최우선으로 둬야하는지?
	// 일단은 다른 애니메이션을 우선으로 하여 현재 진행중인 애니메이션이 없을때만 적용 될 수 있게 조건을 달아줌.

	// 현재 진행중인 애니메이션이 있다면 return
	// TODO: Idle Animaton은 무시하고 실행해야 함

	PLAYER_STATE state = WstringToEnum<PLAYER_STATE>(SM()->GetCurState()->GetName());

	if (/*SM()->Animator3D()->IsActive() &&*/ state != PLAYER_STATE::Player_Idle)
	{
		return;
	}

	m_Delay = 0.07f;
	// 만약 플레이어가 달리는 중이라면
	if (1500.f < m_PlayerScript->GetPlayerVelocity().Length())
	{
		if (KEY_PRESSED(KEY::W))
		{
			m_ClipName = L"Animation\\Armature_run_forward.anim";
		}
		else if (KEY_PRESSED(KEY::A))
		{
			m_ClipName = L"Animation\\Armature_run_left.anim";
		}
		else if (KEY_PRESSED(KEY::S))
		{
			m_ClipName = L"Animation\\Armature_run_backward.anim";
		}
		else if (KEY_PRESSED(KEY::D))
		{
			m_ClipName = L"Animation\\Armature_run_right.anim";
		}
		m_PlayerIdle = false;
		// 애니메이션 적용
		AdjustAnim();
	}
	// 만약 플레이어가 움직이는 중이라면
	else if (1.f < m_PlayerScript->GetPlayerVelocity().Length() && m_PlayerScript->GetPlayerVelocity().Length() < 1500.f)
	{
		MOTION_STATE eCurState = m_PlayerScript->GetMotionState();

		if (KEY_PRESSED(KEY::W))
		{
			if (eCurState == MOTION_STATE::CROUCH)
				m_ClipName = L"Animation\\Armature_walk_crouching_forward.anim";
			else if (eCurState == MOTION_STATE::PRONE)
				m_ClipName = L"Animation\\Armature_prone_left.anim";
			else
				m_ClipName = L"Animation\\Armature_walk_forward.anim";
		}
		else if (KEY_PRESSED(KEY::A))
		{
			if (eCurState == MOTION_STATE::CROUCH)
				m_ClipName = L"Animation\\Armature_walk_crouching_left.anim";
			else if (eCurState == MOTION_STATE::PRONE)
				m_ClipName = L"Animation\\Armature_prone_left.anim";
			else
				m_ClipName = L"Animation\\Armature_walk_left.anim";
		}
		else if (KEY_PRESSED(KEY::S))
		{
			if (eCurState == MOTION_STATE::CROUCH)
				m_ClipName = L"Animation\\Armature_walk_crouching_backward.anim";
			else if (eCurState == MOTION_STATE::PRONE)
				m_ClipName = L"Animation\\Armature_prone_backward.anim";
			else
				m_ClipName = L"Animation\\Armature_walk_backward.anim";
		}
		else if (KEY_PRESSED(KEY::D))
		{
			if (eCurState == MOTION_STATE::CROUCH)
				m_ClipName = L"Animation\\Armature_walk_crouching_right.anim";
			else if (eCurState == MOTION_STATE::PRONE)
				m_ClipName = L"Animation\\Armature_prone_right.anim";
			else
				m_ClipName = L"Animation\\Armature_walk_right.anim";
		}
		m_PlayerIdle = false;
		// 애니메이션 적용
		AdjustAnim();
	}
	else
	{
		MOTION_STATE eCurState = m_PlayerScript->GetMotionState();

		if (!m_PlayerIdle)
		{
			switch (eCurState)
			{
			case MOTION_STATE::STAND:
				m_ClipName = L"Animation\\Armature_idle.anim";
				break;
			case MOTION_STATE::CROUCH:
				m_ClipName = L"Animation\\Armature_idle_crouching.anim";
				break;
			case MOTION_STATE::PRONE:
				m_ClipName = L"Animation\\Armature_prone_idle.anim";
				break;
			}
			m_PlayerIdle = true;
			// 애니메이션 적용
			AdjustAnim();
		}

	}
}
