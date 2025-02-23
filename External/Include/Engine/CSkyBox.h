#pragma once

#include "CRenderComponent.h"
#include "CTexture.h"


enum SKYBOX_MODE
{
    SPHERE,
    CUBE,
};

class CSkyBox :
    public CRenderComponent
{
private:
    Ptr<CTexture>   m_SkyBoxTex;
    SKYBOX_MODE     m_Mode;

public:
    void SetMode(SKYBOX_MODE _Mode);
    void SetSkyBoxTexture(Ptr<CTexture> _Tex) { m_SkyBoxTex = _Tex; }

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

private:
    void CreateMaterial();



public:
    CLONE(CSkyBox);
    CSkyBox();
    ~CSkyBox();
};

