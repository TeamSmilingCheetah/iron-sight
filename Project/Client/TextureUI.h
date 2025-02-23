#pragma once
#include "AssetUI.h"

class TextureUI :
    public AssetUI
{
public:
    void Render_Update() override;

    TextureUI();
    ~TextureUI() override;
};
