#include "pch.h"
#include "System/Public/Rendering/Shader/CComputeShader.h"
#include "System/Public/Manager/CPathMgr.h"
#include "System/Public/Rendering/Buffer/CConstBuffer.h"
#include "System/Public/Rendering/Device/CDevice.h"

CComputeShader::CComputeShader(const wstring& PBlobPath, const wstring& PRelativeFilePath, const wstring& PFunctionName,
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
 * @param PEffectsFilePath Effects File Path (.fx)
 * @param PEntryPointName Effects Entry Point
 * @return 처리 결과
 */
int CComputeShader::CreateComputeShader(const wstring& PBlobFilePath,
                                        const wstring& PEffectsFilePath, const wstring& PEntryPointName)
{
	// Setting Variable
	wstring EffectsFilePath = CPathMgr::GetInst()->GetEffectsFilePath() + PEffectsFilePath;
	wstring BlobFilePath = CPathMgr::GetInst()->GetShaderBlobPath() + PBlobFilePath;

	// Load Compute Shader Blob
	m_CSBlob = LoadBlob(BlobFilePath, EffectsFilePath, PEntryPointName, L"cs_5_0");

	// Early Return
	if (!m_CSBlob)
	{
		return E_FAIL;
	}

	// Create Object
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
	CalculateGroupCount();

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
