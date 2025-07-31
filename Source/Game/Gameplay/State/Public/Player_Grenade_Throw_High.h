#pragma once
#include "Game/Gameplay/State/Public/PlayerState.h"

class Player_Grenade_Throw_High :
	public PlayerState
{
private:


public:
	virtual void Enter_Override() override;
	virtual void FinalTick_Override() override;
	virtual void Exit() override;


public:
	CLONE(Player_Grenade_Throw_High)
	Player_Grenade_Throw_High();
	~Player_Grenade_Throw_High() override;
};

