#pragma once
#include "Client/UI/Public/Component/ComponentUI.h"

class Collider2DUI :
	public ComponentUI
{
public:
	void Render_Update() override;

	Collider2DUI();
	~Collider2DUI() override;
};
