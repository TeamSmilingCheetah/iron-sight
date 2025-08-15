#pragma once
#include "Client/UI/Public/Component/ComponentUI.h"


class ColliderRayUI :
	public ComponentUI
{
public:
	void Render_Update() override;

	ColliderRayUI();
	~ColliderRayUI() override;

};
