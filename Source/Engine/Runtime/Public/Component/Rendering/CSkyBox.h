#pragma once
#include "Engine/Runtime/Public/Component/Base/CRenderComponent.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"

enum SKYBOX_MODE
{
    SPHERE,
    CUBE,
};

class CSkyBox :
    public CRenderComponent
{
    Ptr<CTexture> m_SkyBoxTex;
    SKYBOX_MODE m_Mode;

public:
    void SetMode(SKYBOX_MODE _Mode);
    void SetSkyBoxTexture(Ptr<CTexture> _Tex) { m_SkyBoxTex = _Tex; }

	SKYBOX_MODE GetMode() { return m_Mode; }

    void FinalTick() override;
    void Render() override;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _File) override;

private:
    void CreateMaterial();

public:
    CLONE(CSkyBox);
    CSkyBox();
    ~CSkyBox() override;
};
