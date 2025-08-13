#pragma once
#include "Game/Gameplay/State/Public/PlayerState.h"

class Player_Heal :
	public PlayerState
{
private:

public:
	virtual void Enter_Override() override;
	virtual void FinalTick_Override() override;
	virtual void Exit_Override() override;


public:
	CLONE(Player_Heal)
	Player_Heal();
	~Player_Heal() override;
};

