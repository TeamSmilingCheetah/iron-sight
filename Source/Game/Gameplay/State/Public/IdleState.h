#pragma once

#include "Engine/Runtime/Public/State/CState.h"

class IdleState :
	public CState
{
private:


public:
	virtual void Enter() override;
	virtual void FinalTick() override;
	virtual void Exit() override;


public:
	CLONE(IdleState);
	IdleState();
	~IdleState() override;
};

