#pragma once

#include "Engine/Runtime/Public/State/CState.h"

class GrenadeThrowLowState :
	public CState
{
private:


public:
	virtual void Enter() override;
	virtual void FinalTick() override;
	virtual void Exit() override;


public:
	CLONE(GrenadeThrowLowState);
	GrenadeThrowLowState();
	~GrenadeThrowLowState() override;
};

