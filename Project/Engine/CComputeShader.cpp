#include "pch.h"
#include "CComputeShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CPathMgr.h"

CComputeShader::CComputeShader(const wstring& _RelativeFilePath, const string& _FuncName,
                               int _GroupPerX, int _GroupPerY, int _GroupPerZ)
    : CShader(COMPUTE_SHADER)
      , m_GroupPerThreadX(_GroupPerX)
      , m_GroupPerThreadY(_GroupPerY)
      , m_GroupPerThreadZ(_GroupPerZ)
      , m_GroupX(1)
      , m_GroupY(1)
      , m_GroupZ(1)
{
    if (FAILED(CreateComputeShader(_RelativeFilePath, _FuncName)))
    {
        assert(nullptr);
    }
}

CComputeShader::~CComputeShader()
{
}

int CComputeShader::CreateComputeShader(const wstring& _RelativePath, const string& _FuncName)
{
    wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

    HRESULT hr = S_OK;
    UINT Flag = D3DCOMPILE_DEBUG;

    hr = D3DCompileFromFile(wstring(ContentPath + _RelativePath).c_str()
                            , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
                            , _FuncName.c_str(), "cs_5_0", Flag, 0
                            , m_CSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

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

    DEVICE->CreateComputeShader(m_CSBlob->GetBufferPointer()
                                , m_CSBlob->GetBufferSize()
                                , nullptr, m_CS.GetAddressOf());

    return S_OK;
}

int CComputeShader::Execute()
{
    // CS �� �����Ű�� ���ؼ� �ʿ��� ���ҽ����� ���ε��Ѵ�.
    if (FAILED(Binding()))
    {
        return E_FAIL;
    }

    // �ʿ��� �׷� ���� ����Ѵ�.
    CalcGroupCount();

    // ��� �����͸� ������ۿ� ���� �� ���ε�
    static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
    pCB->SetData(&m_Const);
    pCB->Binding_CS();

    // ��ǻƮ ���̴� ����
    CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);
    CONTEXT->Dispatch(m_GroupX, m_GroupY, m_GroupZ);

    // ���ε��� ���ҽ� ����
    Clear();
}
