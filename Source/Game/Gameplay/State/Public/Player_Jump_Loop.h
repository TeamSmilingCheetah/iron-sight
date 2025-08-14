#pragma once
#include "Game/Gameplay/State/Public/PlayerState.h"

class Player_Jump_Loop :
	public PlayerState
{
private:


public:
	virtual void Enter_Override() override;
	virtual void FinalTick_Override() override;
	virtual void Exit_Override() override;


public:
	CLONE(Player_Jump_Loop)
	Player_Jump_Loop();
	~Player_Jump_Loop() override;
};

