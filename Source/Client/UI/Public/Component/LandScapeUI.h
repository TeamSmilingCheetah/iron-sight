#pragma once
#include "ComponentUI.h"

class LandScapeUI :
	public ComponentUI
{
public:
	virtual void Render_Update() override;
public:
	LandScapeUI();
	~LandScapeUI();
};
