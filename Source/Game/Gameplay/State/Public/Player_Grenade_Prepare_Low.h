#pragma once
#include "Game/Gameplay/State/Public/PlayerState.h"

class Player_Grenade_Prepare_Low :
	public PlayerState
{
private:
	bool m_bInputThrow;
	bool m_bLBTN;

public:
	virtual void Enter_Override() override;
	virtual void FinalTick_Override() override;
	virtual void Exit_Override() override;


public:
	CLONE(Player_Grenade_Prepare_Low)
	Player_Grenade_Prepare_Low();
	~Player_Grenade_Prepare_Low() override;
};

