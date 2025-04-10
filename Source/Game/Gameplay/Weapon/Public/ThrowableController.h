#pragma once
#include "Game/Gameplay/Weapon/Public/WeaponController.h"


class ThrowableController :
	public WeaponController
{
private:

public:
	void Begin() override;
	void Tick() override;

public:
	CLONE(ThrowableController);
	ThrowableController();
	~ThrowableController() override;
};

