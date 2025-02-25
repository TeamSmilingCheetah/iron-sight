#pragma once

#include "AssetUI.h"

class SoundUI
    : public AssetUI
{
public:
    void Render_Update() override;

    SoundUI();
    ~SoundUI() override;
};
