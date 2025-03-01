#include "pch.h"
#include "Rendering/Public/GraphicShaderUI.h"

GraphicShaderUI::GraphicShaderUI()
	: AssetUI("GraphicShader", GRAPHIC_SHADER)
{
}

GraphicShaderUI::~GraphicShaderUI()
{
}

void GraphicShaderUI::Render_Update()
{
	AssetTitle();
}
