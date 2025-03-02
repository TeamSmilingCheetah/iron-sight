#pragma once
#include "Client/UI/Public/Asset/AssetUI.h"

class ComputeShaderUI :
	public AssetUI
{
public:
	void Render_Update() override;

	ComputeShaderUI();
	~ComputeShaderUI() override;
};
