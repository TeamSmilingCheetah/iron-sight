#pragma once
#include "Client/UI/Public/Component/ComponentUI.h"


class RayColliderUI :
	public ComponentUI
{
public:
	void Render_Update() override;

	RayColliderUI();
	~RayColliderUI() override;

};
