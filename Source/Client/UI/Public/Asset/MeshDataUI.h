#pragma once
#include "UI/Public/Asset/AssetUI.h"

class MeshDataUI :
	public AssetUI
{
public:
	void Render_Update() override;

	MeshDataUI();
	~MeshDataUI() override;
};
