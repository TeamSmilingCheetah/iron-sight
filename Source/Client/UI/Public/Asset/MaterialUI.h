#pragma once
#include "Client/UI/Public/Asset/AssetUI.h"

class MaterialUI :
	public AssetUI
{
	TEX_PARAM m_OpenTexType;

public:
	void Render_Update() override;

private:
	void ShaderParameter();
	void SelectGraphicShader(DWORD_PTR _ListUI, DWORD_PTR _SelectString);
	void SelectTexture(DWORD_PTR _ListUI, DWORD_PTR _SelectString);

public:
	MaterialUI();
	~MaterialUI() override;
};
