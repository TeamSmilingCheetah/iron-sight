#pragma once
#include "Client/UI/Public/Component/ComponentUI.h"


class Collider3DUI :
    public ComponentUI
{
public:
	void Render_Update() override;

	Collider3DUI();
	~Collider3DUI() override;

};

