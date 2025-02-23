#pragma once
#include "AssetUI.h"

class MeshDataUI :
    public AssetUI
{
public:
    void Render_Update() override;

    MeshDataUI();
    ~MeshDataUI() override;
};
