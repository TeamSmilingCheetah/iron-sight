#pragma once
#include "System/Public/Rendering/Shader/CComputeShader.h"


class CClearColorCS :
    public CComputeShader
{
    Ptr<CTexture> m_TargetTex;
    Vec4 m_ClearColor;

public:
    void SetTargetTexture(Ptr<CTexture> _Tex) { m_TargetTex = _Tex; }
    void SetClearColor(Vec4 _ClearColor) { m_ClearColor = _ClearColor; }

private:
    int Binding() override;
    void CalcGroupCount() override;
    void Clear() override;

public:
    CClearColorCS();
    ~CClearColorCS() override;
};
