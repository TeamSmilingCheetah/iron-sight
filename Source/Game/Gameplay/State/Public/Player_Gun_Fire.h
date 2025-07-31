#pragma once
#include "Game/Gameplay/State/Public/PlayerState.h"

class Player_Gun_Fire :
	public PlayerState
{
private:


public:
	virtual void Enter_Override() override;
	virtual void FinalTick_Override() override;
	virtual void Exit() override;


public:
	CLONE(Player_Gun_Fire)
	Player_Gun_Fire();
	~Player_Gun_Fire() override;
};

