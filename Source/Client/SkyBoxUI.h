#pragma once
#include "ComponentUI.h"

class SkyBoxUI :
	public ComponentUI
{
public:
	virtual void Render_Update() override;
public:
	SkyBoxUI();
	~SkyBoxUI();
};


