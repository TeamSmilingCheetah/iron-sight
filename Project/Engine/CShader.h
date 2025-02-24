#pragma once
#include "CAsset.h"

class CShader :
    public CAsset
{
protected:
    ComPtr<ID3DBlob> m_ErrBlob;

public:
    int Load(const wstring& _strFilepath) override { return S_OK; }
    int Save(const wstring& _strFilepath) override { return S_OK; }

    CShader(ASSET_TYPE _Type);
    ~CShader() override;
};
