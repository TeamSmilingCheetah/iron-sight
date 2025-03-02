#pragma once
#include "Client/UI/Public/Component/ComponentUI.h"

class Light2DUI :
	public ComponentUI
{
public:
	void Render_Update() override;

	Light2DUI();
	~Light2DUI() override;
};
