#pragma once

#include "Engine/Runtime/Public/State/CState.h"

class GrenadeThrowHighState :
	public CState
{
private:


public:
	virtual void Enter() override;
	virtual void FinalTick() override;
	virtual void Exit() override;


public:
	CLONE(GrenadeThrowHighState);
	GrenadeThrowHighState();
	~GrenadeThrowHighState() override;
};

