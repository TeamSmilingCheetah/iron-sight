#include "pch.h"
#include "CGraphicShader.h"

#include "CPathMgr.h"
#include "CDevice.h"


CGraphicShader::CGraphicShader()
    : CShader(GRAPHIC_SHADER)
      , m_Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
      , m_RSType(RS_TYPE::CULL_BACK)
      , m_BSType(BS_TYPE::DEFAULT)
      , m_DSType(DS_TYPE::LESS)
      , m_StencilRef(0)
      , m_Domain(SHADER_DOMAIN::DOMAIN_NONE)
{
}

CGraphicShader::~CGraphicShader()
{
}

int CGraphicShader::CreateVertexShader(const wstring& _RelativePath, const string& _FuncName)
{
    // ���̴� ������
    wstring ContentPath = CPathMgr::GetInst()->GetContentPath();
    UINT Flag = D3DCOMPILE_DEBUG;
    HRESULT hr = S_OK;

    hr = D3DCompileFromFile(wstring(ContentPath + _RelativePath).c_str()
                            , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
                            , _FuncName.c_str(), "vs_5_0", Flag, 0
                            , m_VSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());
    if (FAILED(hr))
    {
        errno_t errNum = GetLastError();

        if (2 == errNum || 3 == errNum)
        {
            // �߸��� ���
            MessageBoxA(nullptr, "���̴� ������ �������� �ʽ��ϴ�.", "���̴� ������ ����", MB_OK);
        }

        else
        {
            auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
            MessageBoxA(nullptr, pErrMsg, "���̴� ������ ����", MB_OK);
        }

        return E_FAIL;
    }

    DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer()
                               , m_VSBlob->GetBufferSize()
                               , nullptr, m_VS.GetAddressOf());


    // InputLayout
    D3D11_INPUT_ELEMENT_DESC LayoutDesc[8] = {};

    LayoutDesc[0].AlignedByteOffset = 0;
    LayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    LayoutDesc[0].InputSlot = 0;
    LayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    LayoutDesc[0].InstanceDataStepRate = 0;
    LayoutDesc[0].SemanticName = "POSITION";
    LayoutDesc[0].SemanticIndex = 0;

    LayoutDesc[1].AlignedByteOffset = 12;
    LayoutDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    LayoutDesc[1].InputSlot = 0;
    LayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    LayoutDesc[1].InstanceDataStepRate = 0;
    LayoutDesc[1].SemanticName = "TEXCOORD";
    LayoutDesc[1].SemanticIndex = 0;

    LayoutDesc[2].AlignedByteOffset = 20;
    LayoutDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    LayoutDesc[2].InputSlot = 0;
    LayoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    LayoutDesc[2].InstanceDataStepRate = 0;
    LayoutDesc[2].SemanticName = "COLOR";
    LayoutDesc[2].SemanticIndex = 0;

    LayoutDesc[3].AlignedByteOffset = 36;
    LayoutDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    LayoutDesc[3].InputSlot = 0;
    LayoutDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    LayoutDesc[3].InstanceDataStepRate = 0;
    LayoutDesc[3].SemanticName = "TANGENT";
    LayoutDesc[3].SemanticIndex = 0;

    LayoutDesc[4].AlignedByteOffset = 48;
    LayoutDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    LayoutDesc[4].InputSlot = 0;
    LayoutDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    LayoutDesc[4].InstanceDataStepRate = 0;
    LayoutDesc[4].SemanticName = "NORMAL";
    LayoutDesc[4].SemanticIndex = 0;

    LayoutDesc[5].AlignedByteOffset = 60;
    LayoutDesc[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    LayoutDesc[5].InputSlot = 0;
    LayoutDesc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    LayoutDesc[5].InstanceDataStepRate = 0;
    LayoutDesc[5].SemanticName = "BINORMAL";
    LayoutDesc[5].SemanticIndex = 0;

    LayoutDesc[6].AlignedByteOffset = 72;
    LayoutDesc[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    LayoutDesc[6].InputSlot = 0;
    LayoutDesc[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    LayoutDesc[6].InstanceDataStepRate = 0;
    LayoutDesc[6].SemanticName = "BLENDWEIGHT";
    LayoutDesc[6].SemanticIndex = 0;

    LayoutDesc[7].AlignedByteOffset = 88;
    LayoutDesc[7].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    LayoutDesc[7].InputSlot = 0;
    LayoutDesc[7].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    LayoutDesc[7].InstanceDataStepRate = 0;
    LayoutDesc[7].SemanticName = "BLENDINDICES";
    LayoutDesc[7].SemanticIndex = 0;

    if (FAILED(DEVICE->CreateInputLayout(LayoutDesc, 8
        , m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize()
        , m_Layout.GetAddressOf())))
    {
        return E_FAIL;
    }

    return S_OK;
}

int CGraphicShader::CreateHullShader(const wstring& _RelativePath, const string& _FuncName)
{
    wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

    HRESULT hr = S_OK;
    UINT Flag = D3DCOMPILE_DEBUG;

    hr = D3DCompileFromFile(wstring(ContentPath + _RelativePath).c_str()
                            , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
                            , _FuncName.c_str(), "hs_5_0", Flag, 0
                            , m_HSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

    if (FAILED(hr))
    {
        errno_t errNum = GetLastError();

        if (2 == errNum || 3 == errNum)
        {
            // �߸��� ���
            MessageBoxA(nullptr, "���̴� ������ �������� �ʽ��ϴ�.", "���̴� ������ ����", MB_OK);
        }

        else
        {
            auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
            MessageBoxA(nullptr, pErrMsg, "���̴� ������ ����", MB_OK);
        }

        return E_FAIL;
    }

    DEVICE->CreateHullShader(m_HSBlob->GetBufferPointer()
                             , m_HSBlob->GetBufferSize()
                             , nullptr, m_HS.GetAddressOf());

    return S_OK;
}

int CGraphicShader::CreateDomainShader(const wstring& _RelativePath, const string& _FuncName)
{
    wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

    HRESULT hr = S_OK;
    UINT Flag = D3DCOMPILE_DEBUG;

    hr = D3DCompileFromFile(wstring(ContentPath + _RelativePath).c_str()
                            , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
                            , _FuncName.c_str(), "ds_5_0", Flag, 0
                            , m_DSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

    if (FAILED(hr))
    {
        errno_t errNum = GetLastError();

        if (2 == errNum || 3 == errNum)
        {
            // �߸��� ���
            MessageBoxA(nullptr, "���̴� ������ �������� �ʽ��ϴ�.", "���̴� ������ ����", MB_OK);
        }

        else
        {
            auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
            MessageBoxA(nullptr, pErrMsg, "���̴� ������ ����", MB_OK);
        }

        return E_FAIL;
    }

    DEVICE->CreateDomainShader(m_DSBlob->GetBufferPointer()
                               , m_DSBlob->GetBufferSize()
                               , nullptr, m_DS.GetAddressOf());

    return S_OK;
}

int CGraphicShader::CreateGeometryShader(const wstring& _RelativePath, const string& _FuncName)
{
    wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

    HRESULT hr = S_OK;
    UINT Flag = D3DCOMPILE_DEBUG;

    hr = D3DCompileFromFile(wstring(ContentPath + _RelativePath).c_str()
                            , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
                            , _FuncName.c_str(), "gs_5_0", Flag, 0
                            , m_GSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

    if (FAILED(hr))
    {
        errno_t errNum = GetLastError();

        if (2 == errNum || 3 == errNum)
        {
            // �߸��� ���
            MessageBoxA(nullptr, "���̴� ������ �������� �ʽ��ϴ�.", "���̴� ������ ����", MB_OK);
        }

        else
        {
            auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
            MessageBoxA(nullptr, pErrMsg, "���̴� ������ ����", MB_OK);
        }

        return E_FAIL;
    }

    DEVICE->CreateGeometryShader(m_GSBlob->GetBufferPointer()
                                 , m_GSBlob->GetBufferSize()
                                 , nullptr, m_GS.GetAddressOf());

    return S_OK;
}

int CGraphicShader::CreatePixelShader(const wstring& _RelativePath, const string& _FuncName)
{
    wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

    HRESULT hr = S_OK;
    UINT Flag = D3DCOMPILE_DEBUG;

    hr = D3DCompileFromFile(wstring(ContentPath + _RelativePath).c_str()
                            , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
                            , _FuncName.c_str(), "ps_5_0", Flag, 0
                            , m_PSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());
    if (FAILED(hr))
    {
        errno_t errNum = GetLastError();

        if (2 == errNum || 3 == errNum)
        {
            // �߸��� ���
            MessageBoxA(nullptr, "���̴� ������ �������� �ʽ��ϴ�.", "���̴� ������ ����", MB_OK);
        }

        else
        {
            auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
            MessageBoxA(nullptr, pErrMsg, "���̴� ������ ����", MB_OK);
        }

        return E_FAIL;
    }

    DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer()
                              , m_PSBlob->GetBufferSize()
                              , nullptr, m_PS.GetAddressOf());

    return S_OK;
}


void CGraphicShader::Binding()
{
    CONTEXT->IASetPrimitiveTopology(m_Topology); // ������ ����(��ġ, ����)
    CONTEXT->IASetInputLayout(m_Layout.Get());

    CONTEXT->VSSetShader(m_VS.Get(), nullptr, 0);
    CONTEXT->HSSetShader(m_HS.Get(), nullptr, 0);
    CONTEXT->DSSetShader(m_DS.Get(), nullptr, 0);
    CONTEXT->GSSetShader(m_GS.Get(), nullptr, 0);
    CONTEXT->PSSetShader(m_PS.Get(), nullptr, 0);

    // Rstareizer ���ε�
    ComPtr<ID3D11RasterizerState> pRSState = CDevice::GetInst()->GetRSState(m_RSType);
    CONTEXT->RSSetState(pRSState.Get());

    // BlendState ���ε�
    ComPtr<ID3D11BlendState> pBSState = CDevice::GetInst()->GetBSState(m_BSType);
    CONTEXT->OMSetBlendState(pBSState.Get(), nullptr, 0xffffffff);

    // DepthStencilState ���ε�
    ComPtr<ID3D11DepthStencilState> pDSState = CDevice::GetInst()->GetDSState(m_DSType);
    CONTEXT->OMSetDepthStencilState(pDSState.Get(), m_StencilRef);
}
