#pragma once

#include "Game/Gameplay/State/Public/PlayerState.h"

class Player_Gun_Reload :
	public PlayerState
{
private:


public:
	virtual void Enter_Override() override;
	virtual void FinalTick_Override() override;
	virtual void Exit_Override() override;


public:
	CLONE(Player_Gun_Reload)
	Player_Gun_Reload();
	~Player_Gun_Reload() override;
};

