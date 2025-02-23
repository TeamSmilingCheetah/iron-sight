#pragma once
#include "ComponentUI.h"

class Collider2DUI :
    public ComponentUI
{
public:
    void Render_Update() override;

    Collider2DUI();
    ~Collider2DUI() override;
};
