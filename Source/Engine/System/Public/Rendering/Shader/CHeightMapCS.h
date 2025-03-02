#pragma once
#include "Engine/System/Public/Rendering/Shader/CComputeShader.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"

class CStructuredBuffer;

class CHeightMapCS :
    public CComputeShader
{
    Ptr<CTexture> m_HeightMap;
    Ptr<CTexture> m_BrushTex;
    Vec2 m_BrushScale;
    CStructuredBuffer* m_RaycastOut;

public:
    void SetHeightMap(Ptr<CTexture> _Texture) { m_HeightMap = _Texture; }
    void SetBrushPos(CStructuredBuffer* _Buffer) { m_RaycastOut = _Buffer; }
    void SetBrushTex(Ptr<CTexture> _Tex) { m_BrushTex = _Tex; }
    void SetBrushScale(Vec2 _Scale) { m_BrushScale = _Scale; }

    int Binding() override;
    void CalcGroupCount() override;
    void Clear() override;

    CHeightMapCS();
    ~CHeightMapCS() override;
};
