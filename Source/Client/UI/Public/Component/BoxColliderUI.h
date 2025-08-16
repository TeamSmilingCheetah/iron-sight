#pragma once
#include "Client/UI/Public/Component/ComponentUI.h"

class BoxColliderUI :
	public ComponentUI
{
public:
	void Render_Update() override;

	BoxColliderUI();
	~BoxColliderUI() override;
};
