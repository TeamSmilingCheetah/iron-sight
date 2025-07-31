#pragma once
#include "Game/Gameplay/State/Public/PlayerState.h"

class Player_Grenade_Throw_Low :
	public PlayerState
{
private:


public:
	virtual void Enter_Override() override;
	virtual void FinalTick_Override() override;
	virtual void Exit() override;


public:
	CLONE(Player_Grenade_Throw_Low)
	Player_Grenade_Throw_Low();
	~Player_Grenade_Throw_Low() override;
};

