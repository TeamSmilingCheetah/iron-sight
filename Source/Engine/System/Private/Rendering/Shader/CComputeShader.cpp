#include "pch.h"
#include "System/Public/Rendering/Shader/CComputeShader.h"
#include "System/Public/Manager/CPathMgr.h"
#include "System/Public/Rendering/Buffer/CConstBuffer.h"
#include "System/Public/Rendering/Device/CDevice.h"

CComputeShader::CComputeShader(const wstring& PBlobPath, const wstring& PRelativeFilePath, const string& PFunctionName,
                               int PGroupPerX, int PGroupPerY, int PGroupPerZ)
	: CShader(COMPUTE_SHADER)
	  , m_GroupPerThreadX(PGroupPerX)
	  , m_GroupPerThreadY(PGroupPerY)
	  , m_GroupPerThreadZ(PGroupPerZ)
	  , m_GroupX(1)
	  , m_GroupY(1)
	  , m_GroupZ(1)
{
	if (FAILED(CreateComputeShader(PBlobPath, PRelativeFilePath, PFunctionName)))
	{
		assert(nullptr);
	}
}

CComputeShader::~CComputeShader() = default;

/**
 * @brief Compute Shader를 생성하는 함수
 * 생성한 Shader는 m_CSBlob에 저장된다
 * @param PBlobFilePath 컴파일된 Blob File Path (.cso)
 * @param PEffectFilePath Effects File Path (.fx)
 * @param PFunctionName Effects Entry Point
 * @return 처리 결과
 */
int CComputeShader::CreateComputeShader(const wstring& PBlobFilePath,
                                        const wstring& PEffectFilePath, const string& PFunctionName)
{
	HRESULT ResultHandle;
	wstring EffectsFilePath = CPathMgr::GetInst()->GetEffectsFilePath() + PEffectFilePath;
	wstring BlobFilePath = CPathMgr::GetInst()->GetShaderBlobPath() + PBlobFilePath;

#ifdef _DEBUG
	// Compiled Blob이 존재하는 상황이면 바로 로드
	if (exists(BlobFilePath))
	{
#endif

		ResultHandle = D3DReadFileToBlob(wstring(BlobFilePath).c_str(), m_CSBlob.GetAddressOf());
		if (FAILED(ResultHandle))
		{
			assert("Blob 로딩 실패");
			return E_FAIL;
		}

#ifdef _DEBUG
	}
	else
	{
		// 무조건 저장된 경로 혹은 저장될 경로가 존재해야 함
		assert(PBlobFilePath != L"");

		UINT Flag = D3DCOMPILE_DEBUG;

		ResultHandle = D3DCompileFromFile(wstring(EffectsFilePath).c_str()
		                                  , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		                                  , PFunctionName.c_str(), "cs_5_0", Flag, 0
		                                  , m_CSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

		// Error Handle
		if (FAILED(ResultHandle))
		{
			errno_t errNum = static_cast<errno_t>(GetLastError());

			if (2 == errNum || 3 == errNum)
			{
				// Wrong Route
				MessageBoxA(nullptr, "쉐이더 파일이 존재하지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
			}

			else
			{
				auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
				MessageBoxA(nullptr, pErrMsg, "쉐이더 컴파일 실패", MB_OK);
			}

			return E_FAIL;
		}

		// Save Blob
		D3DWriteBlobToFile(m_CSBlob.Get(), wstring(BlobFilePath).c_str(), TRUE);
	}

#endif

	// Device Context에 Compute Shader 생성 처리
	DEVICE->CreateComputeShader(m_CSBlob->GetBufferPointer()
	                            , m_CSBlob->GetBufferSize()
	                            , nullptr, m_CS.GetAddressOf());

	return S_OK;
}

/**
 * Compute Shader의 연산 실행 함수
 * @return 처리 결과
 */
int CComputeShader::Execute()
{
	// Resource Binding
	if (FAILED(Binding()))
	{
		return E_FAIL;
	}

	// Calculate Groups
	CalcGroupCount();

	// Const Buffer Setting & Binding
	static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Const);
	pCB->Binding_CS();

	// Execution
	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);
	CONTEXT->Dispatch(m_GroupX, m_GroupY, m_GroupZ);

	// Clear Resource
	Clear();

	return S_OK;
}
