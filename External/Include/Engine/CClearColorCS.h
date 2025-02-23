#pragma once
#include "CComputeShader.h"



class CClearColorCS :
    public CComputeShader
{
private:
    Ptr<CTexture>   m_TargetTex;
    Vec4            m_ClearColor;

public:
    void SetTargetTexture(Ptr<CTexture> _Tex) { m_TargetTex = _Tex; }
    void SetClearColor(Vec4 _ClearColor) { m_ClearColor = _ClearColor; }

private:
    virtual int Binding() override;
    virtual void CalcGroupCount() override;
    virtual void Clear() override;


public:
    CClearColorCS();
    ~CClearColorCS();
};

