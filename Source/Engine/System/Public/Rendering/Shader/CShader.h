#pragma once
#include "Engine/System/Public/Asset/Base/CAsset.h"

class CShader :
	public CAsset
{
protected:
	ComPtr<ID3DBlob> m_ErrBlob;

public:
	int Load(const wstring& _strFilepath) override { return S_OK; }
	int Save(const wstring& _strFilepath) override { return S_OK; }
	ComPtr<ID3DBlob> LoadBlob(const wstring& PBlobFilePath, const wstring& PEffectsFilePath,
	                          const wstring& PEntryPointName, const wstring& PShaderInfo);

	CShader(ASSET_TYPE PType);
	~CShader() override;
};
