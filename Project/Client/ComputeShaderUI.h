#pragma once

#include "AssetUI.h"

class ComputeShaderUI :
    public AssetUI
{
public:
    void Render_Update() override;

    ComputeShaderUI();
    ~ComputeShaderUI() override;
};
