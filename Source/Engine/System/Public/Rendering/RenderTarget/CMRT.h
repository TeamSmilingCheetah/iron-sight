#pragma once
#include "Core/Public/CEntity.h"
#include "System/Public/Asset/Texture/CTexture.h"

class CMRT :
    public CEntity
{
    Ptr<CTexture> m_Target[8];
    Vec4 m_Clear[8];
    int m_RTCount;

    Ptr<CTexture> m_DSTex;
    D3D11_VIEWPORT m_ViewPort;

public:
    void Create(Ptr<CTexture>* _arrTex, int _Size, Ptr<CTexture> _DSTex);
    void SetClearColor(int _Idx, Vec4 _Color, bool _Independent = true);
    const D3D11_VIEWPORT& GetViewPort() { return m_ViewPort; }
    Ptr<CTexture> GetRenderTarget(int _idx) { return m_Target[_idx]; }

    void Clear();
    void ClearRenderTargets();
    void ClearDepthStencil();
    void OMSet();


    CLONE_DISABLE(CMRT);
    CMRT();
    ~CMRT() override;
};
