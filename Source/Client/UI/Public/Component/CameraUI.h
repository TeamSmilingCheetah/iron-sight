#pragma once
#include "UI/Public/Component/ComponentUI.h"

class CameraUI :
	public ComponentUI
{
public:
	void Render_Update() override;

	CameraUI();
	~CameraUI() override;
};
