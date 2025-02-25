#pragma once
#include "AssetUI.h"

class GraphicShaderUI :
    public AssetUI
{
public:
    void Render_Update() override;

    GraphicShaderUI();
    ~GraphicShaderUI() override;
};
