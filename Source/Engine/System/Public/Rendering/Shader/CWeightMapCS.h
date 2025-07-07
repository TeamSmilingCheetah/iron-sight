#pragma once
#include "Engine/System/Public/Rendering/Shader/CComputeShader.h"

class CTexture;
class CStructuredBuffer;

class CWeightMapCS :
    public CComputeShader
{
    CStructuredBuffer* m_WeightMap;
    Ptr<CTexture> m_BrushTex;
    CStructuredBuffer* m_RaycastOut;
    Vec2 m_BrushScale;
    int m_WeightIdx;

    int m_WeightMapWidth;
    int m_WeightMapHeight;

public:
    void SetBrushPos(CStructuredBuffer* _Buffer) { m_RaycastOut = _Buffer; }
    void SetBrushScale(Vec2 _Scale) { m_BrushScale = _Scale; }
    void SetWeightIdx(int _Idx) { m_WeightIdx = _Idx; }

    void SetWeightMapWidthHeight(UINT _Width, UINT _Height)
    {
        m_WeightMapWidth = static_cast<int>(_Width);
        m_WeightMapHeight = static_cast<int>(_Height);
    }

    void SetWeightMap(CStructuredBuffer* _WeightMap) { m_WeightMap = _WeightMap; }
    void SetBrushTex(Ptr<CTexture> _BrushTex) { m_BrushTex = _BrushTex; }

    int Binding() override;
    void CalculateGroupCount() override;
    void Clear() override;

    CWeightMapCS();
    ~CWeightMapCS() override;
};
