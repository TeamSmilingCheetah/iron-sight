#pragma once
#include "UI/Public/Component/ComponentUI.h"


class TransformUI :
	public ComponentUI
{
public:
	void Render_Update() override;

	TransformUI();
	~TransformUI() override;
};
