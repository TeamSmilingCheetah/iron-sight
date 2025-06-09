#include "pch.h"
#include "System/Public/Manager/CAssetMgr.h"

CAssetMgr::CAssetMgr()
	: m_bAssetChanged(false)
{
}

CAssetMgr::~CAssetMgr()
{
}

// TEST : 메모리 프로파일링 
void CAssetMgr::ReleaseTextureSource()
{
	auto iter = m_mapAsset[(UINT)TEXTURE].begin();

	for (; iter != m_mapAsset[(UINT)TEXTURE].end(); ++iter)
	{
		Ptr<CTexture> pTex = (CTexture*)(iter->second.Get());
		pTex->m_Image.Release();
	}
}

Ptr<CTexture> CAssetMgr::CreateTexture(const wstring& _Key, UINT _Width, UINT _Height,
                                       DXGI_FORMAT _PixelFormat, UINT _BindFlag, D3D11_USAGE _Usage)
{
	Ptr<CTexture> pTex = FindAsset<CTexture>(_Key);

	if (nullptr != pTex)
		return pTex;

	pTex = new CTexture;
	if (FAILED(pTex->Create(_Width, _Height, _PixelFormat, _BindFlag, _Usage)))
	{
		assert(nullptr);
		return nullptr;
	}

	pTex->SetKey(_Key);
	m_mapAsset[static_cast<UINT>(ASSET_TYPE::TEXTURE)].insert(make_pair(_Key, pTex.Get()));
	m_bAssetChanged = true;
	return pTex;
}

Ptr<CTexture> CAssetMgr::CreateTexture(const wstring& _Key, ComPtr<ID3D11Texture2D> _Tex2D)
{
	Ptr<CTexture> pTex = FindAsset<CTexture>(_Key);

	if (nullptr != pTex)
		return pTex;

	pTex = new CTexture;
	if (FAILED(pTex->Create(_Tex2D)))
	{
		assert(nullptr);
		return nullptr;
	}

	pTex->SetKey(_Key);
	m_mapAsset[static_cast<UINT>(ASSET_TYPE::TEXTURE)].insert(make_pair(_Key, pTex.Get()));
	m_bAssetChanged = true;
	return pTex;
}

void CAssetMgr::GetAssetNames(ASSET_TYPE _Type, vector<wstring>& _vecAssetNames)
{
	for (const auto& pair : m_mapAsset[static_cast<UINT>(_Type)])
	{
		_vecAssetNames.push_back(pair.first);
	}
}

void CAssetMgr::DeleteAsset(ASSET_TYPE _Type, const wstring& _Key)
{
	map<wstring, Ptr<CAsset>>& mapAsset = m_mapAsset[static_cast<UINT>(_Type)];

	auto iter = mapAsset.find(_Key);

	if (mapAsset.end() == iter)
		return;

	mapAsset.erase(iter);

	m_bAssetChanged = true;
}

CGameObject* CAssetMgr::ClonePrefabe(const wstring& _Key)
{
	return nullptr;
}

Ptr<CMeshData> CAssetMgr::LoadFBX(const wstring& _strPath)
{
	wstring strFileName = path(_strPath).stem();

	wstring strName = L"MeshData\\";
	strName += strFileName + L".mdat";

	Ptr<CMeshData> pMeshData = FindAsset<CMeshData>(strName);

	if (nullptr != pMeshData)
		return pMeshData;

	pMeshData = CMeshData::LoadFromFBX(_strPath);
	pMeshData->SetKey(strName);
	pMeshData->SetRelativePath(strName);

	m_mapAsset[static_cast<UINT>(ASSET_TYPE::MESH_DATA)].
		insert(make_pair(strName, pMeshData.Get()));

	// meshdata 를 실제파일로 저장
	pMeshData->Save(strName);

	return pMeshData;
}
