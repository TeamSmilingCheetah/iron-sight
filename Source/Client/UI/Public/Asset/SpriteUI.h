#pragma once
#include "Client/UI/Public/Asset/AssetUI.h"

class SpriteUI :
	public AssetUI
{
public:
	void Render_Update() override;

	SpriteUI();
	~SpriteUI() override;
};
