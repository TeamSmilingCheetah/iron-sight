#pragma once
#include "Client/UI/Public/Asset/AssetUI.h"

class SoundUI
	: public AssetUI
{
public:
	void Render_Update() override;

	SoundUI();
	~SoundUI() override;
};
