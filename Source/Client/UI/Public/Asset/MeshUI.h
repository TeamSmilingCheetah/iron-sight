#pragma once
#include "Client/UI/Public/Asset/AssetUI.h"

class MeshUI :
	public AssetUI
{
public:
	void Render_Update() override;

	MeshUI();
	~MeshUI() override;
};
