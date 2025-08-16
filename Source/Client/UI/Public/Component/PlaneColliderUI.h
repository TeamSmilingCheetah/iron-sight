#pragma once
#include "Client/UI/Public/Component/ComponentUI.h"

class PlaneColliderUI :
	public ComponentUI
{
public:
	void Render_Update() override;

	PlaneColliderUI();
	~PlaneColliderUI() override;
};
