#pragma once
#include "Engine/Core/Public/CEntity.h"

class FMultiRenderTarget :
    public CEntity
{
private:
    int MRenderTargetCount;
    Ptr<CTexture> MTargetTexture[8];
    Vec4 MClearColor[8];
    D3D11_VIEWPORT MViewport;
    Ptr<CTexture> MDepthStencilTexture;

public:
    void SetRenderTargets() const;
    void Init(const vector<Ptr<CTexture>>& PTextureVector, const Ptr<CTexture>& PDepthStencilTexture);
    void SetClearColor(int PIdx, const Vec4& PColor);
    void SetClearColorAll(const Vec4& PColor);
    void Clear();

    CLONE_DISABLE(FMultiRenderTarget);
    FMultiRenderTarget() = default;
    ~FMultiRenderTarget() override = default;
};
