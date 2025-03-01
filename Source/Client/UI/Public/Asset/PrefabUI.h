#pragma once
#include "UI/Public/Asset/AssetUI.h"

class PrefabUI :
	public AssetUI
{
public:
	void Render_Update() override;

	PrefabUI();
	~PrefabUI() override;
};
