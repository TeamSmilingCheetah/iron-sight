#pragma once
#include "System/Public/Asset/Base/CAsset.h"

class CAnimation;
class CMaterial;
class CMesh;

class CMeshData :
    public CAsset
{
private:
    vector<Ptr<CMesh>>					m_vecMesh;
    vector<vector<Ptr<CMaterial>>>		m_vecMtrlSet;
	vector<Ptr<CAnimation>>				m_vecAnimSet;	// 같은 본을 사용하는 animation들의 모음

public:
    static CMeshData* LoadFromFBX(const wstring& _RelativePath);

    CGameObject* Instantiate();
    int Save(const wstring& _RelativePath) override;
    int Load(const wstring& _strFilePath) override;

    CLONE_DISABLE(CMeshData);
    CMeshData(bool _Engine = false);
    ~CMeshData() override;
};
