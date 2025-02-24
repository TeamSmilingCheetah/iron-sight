#pragma once
#include "CRenderComponent.h"

#include "assets.h"

class CMeshRender :
    public CRenderComponent
{
public:
    void FinalTick() override;
    void Render() override;

    CLONE(CMeshRender);
    CMeshRender();
    ~CMeshRender() override;
};
