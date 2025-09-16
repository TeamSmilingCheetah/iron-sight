#pragma once
#include "Engine/Runtime/Public/Component/Base/RenderComponent.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"

enum SKYBOX_MODE
{
    SPHERE,
    CUBE,
};

class CSkyBox :
    public FRenderComponent
{
private:
    Ptr<CTexture> m_EnvTex;
    SKYBOX_MODE m_Mode;

	Ptr<CTexture> m_DiffuseTex;
	Ptr<CTexture> m_SpecularTex;
	Ptr<CTexture> m_LookUpTex;

public:
    void SetMode(SKYBOX_MODE _Mode);
    void SetSkyBoxTexture(Ptr<CTexture> _Tex) { m_EnvTex = _Tex; }
	void SetDiffuseTex(Ptr<CTexture> _Tex) { m_DiffuseTex = _Tex; }
	void SetSpecularTex(Ptr<CTexture> _Tex) { m_SpecularTex = _Tex; }
	void SetLUT(Ptr<CTexture> _Tex) { m_LookUpTex = _Tex; }

	SKYBOX_MODE GetMode() const { return m_Mode; }

	Ptr<CTexture> GetDiffuseTex() const { return m_DiffuseTex; }
	Ptr<CTexture> GetSpecularTex() const { return m_SpecularTex; }
	Ptr<CTexture> GetLUT() const { return m_LookUpTex; }

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
