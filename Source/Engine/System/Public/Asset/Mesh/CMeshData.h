#pragma once
#include "Engine/System/Public/Asset/Base/CAsset.h"

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

	vector<Vec3>						m_vecScale;
	vector<Vec3>						m_vecRot;
	vector<Vec3>						m_vecTrans;

public:
    static CMeshData* LoadFromFBX(const wstring& _RelativePath);

    CGameObject* Instantiate();
	void Instantiate(CGameObject* _Obj);

	const vector<Ptr<CAnimation>>& GetAnimations() const { return m_vecAnimSet; }

	// MeshData 편집기능
	void RemoveMesh(int _Idx);
	void RemoveAnimation(int _Idx);

    int Save(const wstring& _RelativePath) override;
    int Load(const wstring& _strFilePath) override;

    CLONE_DISABLE(CMeshData);
    CMeshData(bool _Engine = false);
    ~CMeshData() override;
};
