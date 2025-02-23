#include "pch.h"
#include "GraphicShaderUI.h"

GraphicShaderUI::GraphicShaderUI()
    : AssetUI("GraphicShader", ASSET_TYPE::GRAPHIC_SHADER)
{
}

GraphicShaderUI::~GraphicShaderUI()
{
}

void GraphicShaderUI::Render_Update()
{
    AssetTitle();
}
