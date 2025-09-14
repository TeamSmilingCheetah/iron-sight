#pragma once
#include "Client/UI/Public/Asset/AssetUI.h"

class TextureUI :
	public AssetUI
{
private:
	MetaOpts m_MetaOpts;

public:
	void Render_Update() override;
	void OnSelected() override;

	TextureUI();
	~TextureUI() override;
};
