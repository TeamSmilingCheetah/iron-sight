#pragma once
#include "Client/UI/Public/Asset/AssetUI.h"

class TextureUI :
	public AssetUI
{
public:
	void Render_Update() override;

	TextureUI();
	~TextureUI() override;
};
