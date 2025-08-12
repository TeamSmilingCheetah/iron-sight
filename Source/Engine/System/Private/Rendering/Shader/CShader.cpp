#include "pch.h"
#include "System/Public/Rendering/Shader/CShader.h"

#ifdef _DEBUG
// Debug Build
constexpr UINT CompileFlag = D3DCOMPILE_DEBUG;
#else
// Release Build
constexpr UINT CompileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

CShader::CShader(ASSET_TYPE PType)
	: FAsset(PType, true)
{
}

CShader::~CShader()
{
}

ComPtr<ID3DBlob> CShader::LoadBlob(const wstring& PBlobFilePath, const wstring& PEffectsFilePath,
                                   const wstring& PEntryPointName, const wstring& PShaderInfo)
{
	HRESULT ResultHandle;
	ComPtr<ID3DBlob> ResultBlob;

	// 1. Blob File Exists
	if (exists(PBlobFilePath))
	{
		ResultHandle = D3DReadFileToBlob(wstring(PBlobFilePath).c_str(), ResultBlob.GetAddressOf());
		if (FAILED(ResultHandle))
		{
			assert("Blob 로딩 실패");
		}
	}
	// 2. Need Compile
	else
	{
		assert(PBlobFilePath != L"");

		// Compile Shader
		ResultHandle = D3DCompileFromFile(PEffectsFilePath.c_str(), nullptr
		                                  , D3D_COMPILE_STANDARD_FILE_INCLUDE,
		                                  WStringToString(PEntryPointName).c_str()
		                                  , WStringToString(PShaderInfo).c_str(), CompileFlag, 0
		                                  , ResultBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

		// Error Handle
		if (FAILED(ResultHandle))
		{
			// Exclude Instancing Shader
			if (PEntryPointName.find(L"Inst") == wstring::npos)
			{
				errno_t errNum = static_cast<errno_t>(GetLastError());

				if (2 == errNum || 3 == errNum)
				{
					// 잘못된 경로
					MessageBoxA(nullptr, "쉐이더 파일이 존재하지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
				}
				else
				{
					auto ErrorMessage = static_cast<char*>(m_ErrBlob->GetBufferPointer());
					MessageBoxA(nullptr, ErrorMessage, "쉐이더 컴파일 실패", MB_OK);
				}
			}

			return nullptr;
		}

		// Save Blob
		D3DWriteBlobToFile(ResultBlob.Get(), wstring(PBlobFilePath).c_str(), TRUE);
	}

	return ResultBlob;
}
