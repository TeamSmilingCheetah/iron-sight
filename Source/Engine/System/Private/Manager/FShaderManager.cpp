#include "pch.h"
#include "Engine/System/Public/Manager/FShaderManager.h"

#include "System/Public/Manager/FShaderInfoMap.h"

#ifdef _DEBUG
// Debug Build
constexpr UINT CompileFlag = D3DCOMPILE_DEBUG;
#else
// Release Build
constexpr UINT CompileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

FShaderManager::FShaderManager() = default;

FShaderManager::~FShaderManager() = default;

/**
 * @brief ShaderManager를 처음 초기화할 때 호출하는 함수
 * ShaderInfoMap은 초기화 과정에서만 필요하므로 로드해서 사용한 뒤, 제거한다
 */
void FShaderManager::Init()
{
	MShaderInfoMap = new FShaderInfoMap();
	// ShaderID::COUNT 크기만큼 BlobVec 초기화
	MShaderBlobVec.resize(static_cast<size_t>(ShaderID::COUNT));
	wstring EffectsFolderPath = CPathMgr::GetInst()->GetShaderBlobFolderPath();
	BuildAllShaders(EffectsFolderPath);
	delete MShaderInfoMap;
}

/**
 * @brief 미리 빌드하기로 기록한 셰이더들을 전부 빌드 시도하는 함수
 *
 * @param PFolderPath CSO 파일이 위치한 경로
 */
void FShaderManager::BuildAllShaders(const wstring& PFolderPath)
{
    for (size_t i = 0; i < static_cast<size_t>(ShaderID::COUNT); ++i)
    {
        const ShaderInfo& info = MShaderInfoMap->Data[i];
        BuildShader(static_cast<ShaderID>(i), info, PFolderPath);
    }
}

/**
 * @brief 단일 셰이더에 대해서 빌드 및 캐싱하는 함수
 * 기본적으로는 컴파일된 적절한 CSO 오브젝트를 가져오고, 이를 자료구조에 캐싱한다
 * @param PFullCSOPath .cso 파일이 위치한 전체 경로
 * @param PShaderInfo Shader 정보
 */
void FShaderManager::BuildShader(ShaderID id, const ShaderInfo& PShaderInfo, const wstring& PFolderPath)
{
    wstring FullPath = path(PFolderPath) / PShaderInfo.CsoFileName;
    // Get Blob By Compile Function
    wstring Prefix = PShaderInfo.EntryPoint.substr(0, 2);
    wstring ShaderModel;
    if (Prefix == L"VS")      ShaderModel = L"vs_5_0";
    else if (Prefix == L"PS") ShaderModel = L"ps_5_0";
    else if (Prefix == L"GS") ShaderModel = L"gs_5_0";
    else if (Prefix == L"CS") ShaderModel = L"cs_5_0";
    else if (Prefix == L"HS") ShaderModel = L"hs_5_0";
    else if (Prefix == L"DS") ShaderModel = L"ds_5_0";
    else { assert(!"맞는 셰이더 모델이 존재하지 않음"); }
    wstring EffectsFolderPath = CPathMgr::GetInst()->GetEffectsFolderPath();
    wstring EffectsFullPath = path(EffectsFolderPath) / PShaderInfo.EffectsFileName;
    auto Blob = GetCompiledBlob(FullPath, EffectsFullPath, PShaderInfo.EntryPoint, ShaderModel);
    if (Blob)
        RegisterShader(id, Blob);
    else
        assert(!"셰이더 초기화 빌드에 문제가 발생함");
}

/**
 * ShaderManager의 멤버인 BlobMap에 ShaderBlob을 캐싱하는 함수
 * @param PCSOName 파일 이름 (Key)
 * @param PShaderBlob 셰이더 오브젝트 (Value)
 */
void FShaderManager::RegisterShader(ShaderID id, ComPtr<ID3DBlob> PShaderBlob)
{
    MShaderBlobVec[static_cast<size_t>(id)] = PShaderBlob;
}

ComPtr<ID3DBlob> FShaderManager::GetBlob(ShaderID id)
{
    return MShaderBlobVec[static_cast<size_t>(id)];
}

/**
 * @brief CSO Object를 .cso 파일이나 .fx 파일로부터 가져오는 함수
 *
 * @param PBlobFilePath Blob 파일의 상대 경로
 * @param PEffectsFilePath Effects (.fx) 파일의 상대 경로
 * @param PEntryPointName Effects 파일 내 함수 Entry Point
 * @param PShaderInfo
 * @return Compiled Shader Object의 Pointer
 */
ComPtr<ID3DBlob> FShaderManager::GetCompiledBlob(const wstring& PBlobFilePath, const wstring& PEffectsFilePath,
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
	// 2. Blob File Doesn't Exist & Need Compile
	else
	{
		if (PEffectsFilePath != L"")
		{
			assert("컴파일된 Shader 오브젝트도 없는데, Effects 파일도 존재하지 않음");
		}

		// Compile Shader
		ResultHandle = D3DCompileFromFile(PEffectsFilePath.c_str(), nullptr
		                                  , D3D_COMPILE_STANDARD_FILE_INCLUDE,
		                                  WStringToString(PEntryPointName).c_str()
		                                  , WStringToString(PShaderInfo).c_str(), CompileFlag, 0
		                                  , ResultBlob.GetAddressOf(), MErrBlob.GetAddressOf());

		// Error Handle
		if (FAILED(ResultHandle))
		{
			// Exclude Instancing Shader
			if (PEntryPointName.find(L"inst") == wstring::npos)
			{
				errno_t ErrorNumber = static_cast<errno_t>(GetLastError());

				if (2 == ErrorNumber || 3 == ErrorNumber)
				{
					// 잘못된 경로
					MessageBoxA(nullptr, "쉐이더 파일이 존재하지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
				}
				else
				{
					auto ErrorMessage = static_cast<char*>(MErrBlob->GetBufferPointer());
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
