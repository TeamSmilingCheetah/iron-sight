#pragma once
#include "CAsset.h"

#include "CMesh.h"
#include "CMaterial.h"

class CMeshData :
    public CAsset
{
    vector<Ptr<CMesh>>				m_vecMesh;
    vector<vector<Ptr<CMaterial>>>	m_vecMtrlSet;

public:
    static CMeshData* LoadFromFBX(const wstring& _RelativePath);

    CGameObject* Instantiate();
    int Save(const wstring& _RelativePath) override;
    int Load(const wstring& _strFilePath) override;

    CLONE_DISABLE(CMeshData);
    CMeshData(bool _Engine = false);
    ~CMeshData() override;
};
