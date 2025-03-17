#pragma once
#include "ComponentUI.h"

class Animator3DUI :
	public ComponentUI
{
public:
	virtual void Render_Update() override;
public:
	Animator3DUI();
	~Animator3DUI();
};

