#pragma once
#include "AssetUI.h"

class FlipbookUI :
    public AssetUI
{
public:
    void Render_Update() override;

    FlipbookUI();
    ~FlipbookUI() override;
};
