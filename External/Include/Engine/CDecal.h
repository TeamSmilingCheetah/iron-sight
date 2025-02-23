#pragma once
#include "CRenderComponent.h"

#include "CTexture.h"

class CDecal :
    public CRenderComponent
{
private:
    Ptr<CTexture>   m_DecalTex;
    float           m_GlobalAlpha;
    bool            m_AsLight;


public:
    void SetDecalTexture(Ptr<CTexture> _Tex) { m_DecalTex = _Tex; }
    void SetGloablAlpha(float _Alpha) { m_GlobalAlpha = _Alpha; }

    Ptr<CTexture> GetDecalTex() { return m_DecalTex; }
    float GetGlobalAlpha() { return m_GlobalAlpha; }

    void SetAsLight(bool _bLight) { m_AsLight = _bLight; }

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

private:
    void CreateMaterial();

public:
    CLONE(CDecal);
    CDecal();
    ~CDecal();
};

