#pragma once
#include "Client/UI/Public/Component/ComponentUI.h"

class DecalUI :
	public ComponentUI
{
public:
	void Render_Update() override;

	DecalUI();
	~DecalUI() override;
};
