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
            // 잘못된 경로
            MessageBoxA(nullptr, "쉐이더 파일이 존재하지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
        }

        else
        {
            auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
            MessageBoxA(nullptr, pErrMsg, "쉐이더 컴파일 실패", MB_OK);
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
    // CS 를 실행시키기 위해서 필요한 리소스들을 바인딩한다.
    if (FAILED(Binding()))
    {
        return E_FAIL;
    }

    // 필요한 그룹 수를 계산한다.
    CalcGroupCount();

    // 상수 데이터를 상수버퍼에 전달 및 바인딩
    static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
    pCB->SetData(&m_Const);
    pCB->Binding_CS();

    // 컴퓨트 쉐이더 실행
    CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);
    CONTEXT->Dispatch(m_GroupX, m_GroupY, m_GroupZ);

    // 바인딩한 리소스 정리
    Clear();
}
