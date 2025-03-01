#pragma once
#include "UI/Public/Asset/AssetUI.h"

class FlipbookUI :
	public AssetUI
{
public:
	void Render_Update() override;

	FlipbookUI();
	~FlipbookUI() override;
};
