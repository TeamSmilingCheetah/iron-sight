#pragma once
#include "Game/Gameplay/State/Public/PlayerState.h"

class Player_Dead :
	public PlayerState
{
private:
	float	m_AccTime;
	bool	m_bCamFlag;

public:
	virtual void Enter_Override() override;
	virtual void FinalTick_Override() override;
	virtual void Exit_Override() override;


public:
	CLONE(Player_Dead)
	Player_Dead();
	~Player_Dead() override;
};

