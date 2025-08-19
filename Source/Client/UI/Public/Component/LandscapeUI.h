#pragma once
#include "ComponentUI.h"

class LandscapeUI :
	public ComponentUI
{
public:
	virtual void Render_Update() override;
public:
	LandscapeUI();
	~LandscapeUI();
};
