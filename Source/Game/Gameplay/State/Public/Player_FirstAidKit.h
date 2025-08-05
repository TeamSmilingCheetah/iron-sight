#pragma once
#include "Game/Gameplay/State/Public/PlayerState.h"

class Player_FirstAidKit :
	public PlayerState
{
private:


public:
	virtual void Enter_Override() override;
	virtual void FinalTick_Override() override;
	virtual void Exit() override;


public:
	CLONE(Player_FirstAidKit)
	Player_FirstAidKit();
	~Player_FirstAidKit() override;
};

