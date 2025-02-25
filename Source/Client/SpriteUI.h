#pragma once
#include "AssetUI.h"

class SpriteUI :
    public AssetUI
{
public:
    void Render_Update() override;

    SpriteUI();
    ~SpriteUI() override;
};
