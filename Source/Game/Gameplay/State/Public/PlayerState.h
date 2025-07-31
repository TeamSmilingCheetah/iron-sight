#pragma once
#include "Engine/Runtime/Public/State/CState.h"
#include "Engine/System/Public/Asset/Animation/CAnimation.h"

class PlayerCharacter;

// 모든 Player State들의 부모 class. 애니메이션 컨트롤을 공통적으로 적용하기 위해 설계함.
class PlayerState
	: public CState
{
private:
	PlayerCharacter*	m_PlayerScript;
	bool				m_CanExitDuringAnimation;

public:
	PlayerCharacter* GetPlayerScript() const { return m_PlayerScript; }
	void SetCanExitDuringAnimation(bool _b) { m_CanExitDuringAnimation = _b; }

	virtual void Enter() final;
	virtual void FinalTick() final;
	virtual void Exit() = 0;

	virtual void Enter_Override() = 0;
	virtual void FinalTick_Override() = 0;

public:
	virtual CState* Clone() = 0;
	PlayerState(const wstring& _Name);
	~PlayerState();
};
