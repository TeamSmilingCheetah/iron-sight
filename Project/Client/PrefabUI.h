#pragma once
#include "AssetUI.h"

class PrefabUI :
    public AssetUI
{
public:
    void Render_Update() override;

    PrefabUI();
    ~PrefabUI() override;
};
