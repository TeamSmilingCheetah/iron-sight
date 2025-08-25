#pragma once
#include "Game/Gameplay/State/Public/PlayerState.h"

class Player_Idle :
	public PlayerState
{
private:


public:
	virtual void Enter_Override() override;
	virtual void FinalTick_Override() override;
	virtual void Exit_Override() override;

private:
	void Play_Idle_Anim();

public:
	CLONE(Player_Idle)
	Player_Idle();
	~Player_Idle() override;
};

