#pragma once
#include "Engine/Runtime/Public/Component/Base/CRenderComponent.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"

class CDecal :
    public CRenderComponent
{
private:
    Ptr<CTexture> m_DecalTex;
    float m_GlobalAlpha;
	float m_AccTime;
	float m_LifeTime;
	bool m_AsLight;

public:
    void SetDecalTexture(Ptr<CTexture> _Tex) { m_DecalTex = _Tex; }
    void SetGloablAlpha(float _Alpha) { m_GlobalAlpha = _Alpha; }

    Ptr<CTexture> GetDecalTex() { return m_DecalTex; }
    float GetGlobalAlpha() { return m_GlobalAlpha; }

    void SetAsLight(bool _bLight) { m_AsLight = _bLight; }
	bool IsAsLight() { return m_AsLight; }

	void SetLifeTime(float _LifeTime) { m_LifeTime = _LifeTime; }

    void FinalTick() override;
    void Render() override;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _File) override;

private:
    void CreateMaterial();

public:
    CLONE(CDecal);
    CDecal();
    ~CDecal() override;
};
