#pragma once

#include "Engine/Runtime/Public/State/CState.h"

class BandageState :
	public CState
{
private:


public:
	virtual void Enter() override;
	virtual void FinalTick() override;
	virtual void Exit() override;


public:
	CLONE(BandageState);
	BandageState();
	~BandageState() override;
};

