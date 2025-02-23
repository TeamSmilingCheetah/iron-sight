#include "pch.h"
#include "CTexture.h"

#include "CDevice.h"
#include "CPathMgr.h"

CTexture::CTexture()
    : CAsset(TEXTURE, false)
      , m_Desc{}
      , m_RecentSRVNum(-1)
      , m_RecentUAVNum(-1)
{
}

CTexture::~CTexture()
{
}


int CTexture::Load(const wstring& _FilePath)
{
    // ���� -> SystemMem
    wchar_t szExt[50] = {};
    _wsplitpath_s(_FilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);
    wstring strExt = szExt;

    HRESULT hr = E_FAIL;

    if (strExt == L".dds" || strExt == L".DDS")
    {
        // DDS
        hr = LoadFromDDSFile(_FilePath.c_str(), DDS_FLAGS_NONE, nullptr, m_Image);
    }
    else if (strExt == L".tga" || strExt == L".TGA")
    {
        // TGA
        hr = LoadFromTGAFile(_FilePath.c_str(), nullptr, m_Image);
    }
    else
    {
        // Window Image Component(WIC)
        // png, jpg, jpeg, bmp
        hr = LoadFromWICFile(_FilePath.c_str(), WIC_FLAGS_NONE, nullptr, m_Image);
    }

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"�ؽ��� �ε� ����", L"���ҽ� �ε� ����", MB_OK);
        return E_FAIL;
    }

    // System -> GPU
    // m_Image -> m_Tex2D
    // Texture2D ��ü�� ����
    // Texture2D �� �����Ҷ� ����� ShaderResourceView
    CreateShaderResourceView(DEVICE
                             , m_Image.GetImages()
                             , m_Image.GetImageCount()
                             , m_Image.GetMetadata(), m_SRV.GetAddressOf());

    // ������ ShaderResourveView �� �̿��ؼ� ���� ��ü(Texture2D) �� �˾Ƴ���.
    m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());

    // ������ Texture2D �� Desc ������ �˾Ƴ���.
    m_Tex2D->GetDesc(&m_Desc);

    return S_OK;
}

int CTexture::Save(const wstring& _RelativePath)
{
    // GPU -> System
    CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);

    // System -> File	
    SetRelativePath(_RelativePath);

    wstring FilePath = CPathMgr::GetInst()->GetContentPath() + _RelativePath;

    HRESULT hr = E_FAIL;
    if (1 == m_Image.GetMetadata().arraySize)
    {
        // png, jpg, jpeg, bmp, 
        hr = SaveToWICFile(*m_Image.GetImages()
                           , WIC_FLAGS_NONE
                           , GetWICCodec(WIC_CODEC_PNG)
                           , FilePath.c_str());
    }

    else
    {
        hr = SaveToDDSFile(m_Image.GetImages()
                           , m_Image.GetMetadata().arraySize
                           , m_Image.GetMetadata()
                           , DDS_FLAGS_NONE
                           , FilePath.c_str());
    }

    return hr;
}

int CTexture::Create(UINT _Width, UINT _Height, DXGI_FORMAT _PixelFormat, UINT _BindFlag,
                     D3D11_USAGE _Usage)
{
    m_Desc.Width = _Width;
    m_Desc.Height = _Height;

    m_Desc.ArraySize = 1;
    m_Desc.Format = _PixelFormat;

    // �ؽ����� �뵵
    m_Desc.BindFlags = _BindFlag;

    // CPU ���� ���� ���Ŀ� ������ �������� �ɼ�
    m_Desc.Usage = _Usage;

    if (m_Desc.Usage == D3D11_USAGE_DYNAMIC)
        m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    else if (m_Desc.Usage == D3D11_USAGE_STAGING)
        m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    // �Ӹ� ������ 1 ==> ������ ������ 
    m_Desc.MipLevels = 1;

    m_Desc.MiscFlags = 0;
    m_Desc.SampleDesc.Count = 1;
    m_Desc.SampleDesc.Quality = 0;

    if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
    {
        return E_FAIL;
    }


    // View ����	
    if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
    {
        if (FAILED(DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf())))
        {
            return E_FAIL;
        }
    }

    else
    {
        if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
        {
            if (FAILED(
                DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf())))
            {
                return E_FAIL;
            }
        }

        if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        {
            if (FAILED(
                DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf())))
            {
                return E_FAIL;
            }
        }

        if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
        {
            if (FAILED(
                DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), nullptr, m_UAV.GetAddressOf())))
            {
                return E_FAIL;
            }
        }
    }

    return S_OK;
}

int CTexture::Create(ComPtr<ID3D11Texture2D> _Tex2D)
{
    assert(_Tex2D.Get());

    m_Tex2D = _Tex2D;
    m_Tex2D->GetDesc(&m_Desc);

    // View ����	
    if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
    {
        if (FAILED(DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf())))
        {
            return E_FAIL;
        }
    }

    else
    {
        if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
        {
            if (FAILED(
                DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf())))
            {
                return E_FAIL;
            }
        }

        if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        {
            if (FAILED(
                DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf())))
            {
                return E_FAIL;
            }
        }
    }

    return S_OK;
}

int CTexture::CreateArrayTexture(const vector<Ptr<CTexture>>& _vecTex)
{
    m_Desc = _vecTex[0]->GetDesc();
    m_Desc.ArraySize = static_cast<UINT>(_vecTex.size());
    m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    m_Desc.MipLevels = 1;

    if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
    {
        return E_FAIL;
    }

    // ���� �� �ؽ��ĸ� ������ �迭 �ؽ����� �� ĭ���� �����Ų��.
    for (size_t i = 0; i < _vecTex.size(); ++i)
    {
        UINT Offset = D3D11CalcSubresource(0, i, 1);

        CONTEXT->UpdateSubresource(m_Tex2D.Get(), Offset, nullptr
                                   , _vecTex[i]->GetPixels()
                                   , _vecTex[i]->GetRowPitch()
                                   , _vecTex[i]->GetSlicePitch());
    }

    // Shader Resrouce View ����
    D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};

    tSRVDesc.Format = m_Desc.Format;
    tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    tSRVDesc.Texture2DArray.MipLevels = 1;
    tSRVDesc.Texture2DArray.MostDetailedMip = 0;
    tSRVDesc.Texture2DArray.ArraySize = m_Desc.ArraySize;

    if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &tSRVDesc, m_SRV.GetAddressOf())))
        return E_FAIL;

    return S_OK;
}

int CTexture::GenerateMip(UINT _Level)
{
    // CubeTexture �� Mipmap ���� ����
    assert(false == m_Desc.MiscFlags & D3D11_SRV_DIMENSION_TEXTURECUBE);

    m_Tex2D = nullptr;
    m_RTV = nullptr;
    m_DSV = nullptr;
    m_SRV = nullptr;
    m_UAV = nullptr;

    m_Desc.MipLevels = _Level;
    m_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    m_Desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

    if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
    {
        return E_FAIL;
    }

    for (UINT i = 0; i < m_Desc.ArraySize; ++i)
    {
        UINT iSubIdx = D3D11CalcSubresource(0, i, m_Desc.MipLevels);

        CONTEXT->UpdateSubresource(m_Tex2D.Get(), iSubIdx, nullptr
                                   , m_Image.GetImage(0, i, 0)->pixels
                                   , m_Image.GetImage(0, i, 0)->rowPitch
                                   , m_Image.GetImage(0, i, 0)->slicePitch);
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

    if (2 <= m_Desc.ArraySize)
    {
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        SRVDesc.Texture2DArray.ArraySize = m_Desc.ArraySize;
        SRVDesc.Texture2DArray.MipLevels = m_Desc.MipLevels;
        SRVDesc.Texture2DArray.MostDetailedMip = 0;
    }
    else
    {
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MipLevels = m_Desc.MipLevels;
        SRVDesc.Texture2D.MostDetailedMip = 0;
    }

    if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &SRVDesc, m_SRV.GetAddressOf())))
        return E_FAIL;

    CONTEXT->GenerateMips(m_SRV.Get());

    return S_OK;
}

void CTexture::Binding(int _RegisterNum)
{
    CONTEXT->VSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->HSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->DSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->GSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->PSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void CTexture::Clear(int _RegisterNum)
{
    ID3D11ShaderResourceView* pSRV = nullptr;
    CONTEXT->VSSetShaderResources(_RegisterNum, 1, &pSRV);
    CONTEXT->HSSetShaderResources(_RegisterNum, 1, &pSRV);
    CONTEXT->DSSetShaderResources(_RegisterNum, 1, &pSRV);
    CONTEXT->GSSetShaderResources(_RegisterNum, 1, &pSRV);
    CONTEXT->PSSetShaderResources(_RegisterNum, 1, &pSRV);
}

void CTexture::Binding_SRV_CS(int _RegisterNum)
{
    m_RecentSRVNum = _RegisterNum;
    CONTEXT->CSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void CTexture::Binding_UAV_CS(int _RegisterNum)
{
    assert(m_UAV.Get());

    m_RecentUAVNum = _RegisterNum;
    UINT i = -1;
    CONTEXT->CSSetUnorderedAccessViews(_RegisterNum, 1, m_UAV.GetAddressOf(), &i);
}

void CTexture::Clear_SRV_CS()
{
    ID3D11ShaderResourceView* pSRV = nullptr;
    CONTEXT->CSSetShaderResources(m_RecentSRVNum, 1, &pSRV);
}

void CTexture::Clear_UAV_CS()
{
    ID3D11UnorderedAccessView* pUAV = nullptr;
    UINT i = -1;
    CONTEXT->CSSetUnorderedAccessViews(m_RecentUAVNum, 1, &pUAV, &i);
}
