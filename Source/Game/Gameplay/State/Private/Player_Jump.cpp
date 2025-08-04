#include "pch.h"
#include "Game/Gameplay/State/Public/Player_Jump.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"


Player_Jump::Player_Jump()
	: PlayerState(L"Player_Jump")
{
	SetCanExitDuringAnimation(false);
}

Player_Jump::~Player_Jump()
{
}

void Player_Jump::Enter_Override()
{
	m_Delay = 0.02f;
	m_ClipName = L"Animation\\Armature_rifle_jump.anim";

	AdjustAnim();
}

void Player_Jump::FinalTick_Override()
{
	// StateMachine 의 CanExit가 false라서 State 전환이 안됨 (애니메이션이 loop 설정 어디서 해주는지?)
	// 아니면 State Tick에서 시간 계산해서 수동으로 bool값 설정 해줘야 하는지?
	if (GetPlayerScript()->IsGround())
	{
		SM()->SetChange(L"Player_Idle");
		//SM()->ChangeState(L"Player_Idle");
	}
}

void Player_Jump::Exit()
{
}
