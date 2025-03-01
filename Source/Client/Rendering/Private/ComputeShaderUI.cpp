#include "pch.h"
#include "Rendering/Public/ComputeShaderUI.h"

ComputeShaderUI::ComputeShaderUI()
	: AssetUI("ComputeShader", COMPUTE_SHADER)
{
}

ComputeShaderUI::~ComputeShaderUI()
{
}

void ComputeShaderUI::Render_Update()
{
	AssetTitle();
}
