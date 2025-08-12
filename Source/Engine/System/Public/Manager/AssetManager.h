#pragma once
#include "Common/singleton.h"
#include "Engine/System/Public/Asset/Base/assets.h"
#include "Engine/System/Public/Manager/CPathMgr.h"

class FAssetManager :
	public singleton<FAssetManager>
{
	SINGLE(FAssetManager)

private:
	map<wstring, Ptr<CAsset>> m_mapAsset[static_cast<UINT>(ASSET_TYPE::END)];
	bool m_bAssetChanged;

public:
	void Init();
	void Shutdown();

	bool IsAssetChanged()
	{
		bool Changed = m_bAssetChanged;
		m_bAssetChanged = false;
		return Changed;
	}

private:
	void CreateEngineMesh();
	void CreateEngineTexture();
	void CreateEngineGraphicShader();
	void CreateEngineComputeShader();
	void CreateEngineMaterial();
	void CreateEngineSprite();

public:
	template <typename T>
	int AddAsset(const wstring& _Key, Ptr<T> _Asset);

	template <typename T>
	Ptr<T> FindAsset(const wstring& _Key);

	template <typename T>
	Ptr<T> Load(const wstring& _Key, const wstring& _RelativePath);

	template <typename T>
	Ptr<T> Load(const wstring& _RelativePath);

	template <>
	Ptr<CComputeShader> Load(const wstring& _Key, const wstring& _RelativePath)
	{
		return FindAsset<CComputeShader>(_Key).Get();
	}

	bool ChangeAssetKey(Ptr<CAsset> _Asset, const wstring& _NewKey);

	Ptr<CTexture> CreateTexture(const wstring& _Key, UINT _Width, UINT _Height,
								DXGI_FORMAT _PixelFormat, UINT _BindFlag,
								D3D11_USAGE _Usage = D3D11_USAGE_DEFAULT);
	Ptr<CTexture> CreateTexture(const wstring& _Key, ComPtr<ID3D11Texture2D> _Tex2D);

	Ptr<CMeshData> LoadFBX(const wstring& _strPath);

	void GetAssetNames(ASSET_TYPE _Type, vector<wstring>& _vecAssetNames);

	const map<wstring, Ptr<CAsset>>& GetAssets(ASSET_TYPE _Type)
	{
		return m_mapAsset[static_cast<UINT>(_Type)];
	}

	void DeleteAsset(ASSET_TYPE _Type, const wstring& _Key);

	Ptr<CAsset> CopyAsset(Ptr<CAsset> _Source);
};

template <typename T>
ASSET_TYPE GetAssetType()
{
	if constexpr (std::is_same_v<T, CMesh>)
		return MESH;
	if constexpr (std::is_same_v<T, CGraphicShader>)
		return GRAPHIC_SHADER;
	if constexpr (std::is_same_v<T, CComputeShader>)
		return COMPUTE_SHADER;
	if constexpr (std::is_same_v<T, CMeshData>)
		return MESH_DATA;
	if constexpr (std::is_same_v<T, CMaterial>)
		return MATERIAL;
	if constexpr (std::is_same_v<T, CPrefab>)
		return PREFAB;
	if constexpr (std::is_same_v<T, CSound>)
		return SOUND;
	if constexpr (std::is_same_v<T, CFlipbook>)
		return FLIPBOOK;
	if constexpr (std::is_same_v<T, CSprite>)
		return SPRITE;
	if constexpr (std::is_same_v<T, CTexture>)
		return TEXTURE;
	if constexpr (std::is_same_v<T, CAnimation>)
		return ANIMATION;
	if constexpr (std::is_same_v<T, CSkeleton>)
		return SKELETON;

	return END;
}

template <typename T>
int FAssetManager::AddAsset(const wstring& _Key, Ptr<T> _Asset)
{
	static_assert(std::is_base_of<CAsset, T>::value, "T must derive from CAsset");

	ASSET_TYPE Type = _Asset->GetAssetType();

	_Asset->SetKey(_Key);
	m_mapAsset[static_cast<UINT>(Type)].insert(make_pair(_Key, _Asset.Get()));

	m_bAssetChanged = true;

	return S_OK;
}

template <typename T>
Ptr<T> FAssetManager::FindAsset(const wstring& _Key)
{
	ASSET_TYPE Type = GetAssetType<T>();

	auto iter = m_mapAsset[static_cast<UINT>(Type)].find(_Key);

	if (iter == m_mapAsset[static_cast<UINT>(Type)].end())
		return nullptr;

	return static_cast<T*>(iter->second.Get());
}

template <typename T>
Ptr<T> FAssetManager::Load(const wstring& _Key, const wstring& _RelativePath)
{
	Ptr<CAsset> pAsset = FindAsset<T>(_Key).Get();

	if (nullptr != pAsset)
		return static_cast<T*>(pAsset.Get());

	// 텍스쳐 파일 경로
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _RelativePath;

	// 에셋 객체 생성 및 로딩
	pAsset = new T;
	if (FAILED(pAsset->Load(strFilePath)))
	{
		pAsset = nullptr;
		return nullptr;
	}

	// 로딩이 완료된 에셋에 본인의 Key, RelativePath 세팅
	pAsset->SetKey(_Key);
	pAsset->SetRelativePath(_RelativePath);

	// 에셋을 맵에 등록
	ASSET_TYPE Type = GetAssetType<T>();
	m_mapAsset[static_cast<UINT>(Type)].insert(make_pair(_Key, pAsset));

	m_bAssetChanged = true;

	return static_cast<T*>(pAsset.Get());
}


// TEST: Engine Resource가 아닌 경우 key와 relative path가 동일하다고 가정
template <typename T>
Ptr<T> FAssetManager::Load(const wstring& _RelativePath)
{
	Ptr<CAsset> pAsset = FindAsset<T>(_RelativePath).Get();

	if (nullptr != pAsset)
		return static_cast<T*>(pAsset.Get());

	// 텍스쳐 파일 경로
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _RelativePath;

	// 에셋 객체 생성 및 로딩
	pAsset = new T;
	if (FAILED(pAsset->Load(strFilePath)))
	{
		pAsset = nullptr;
		return nullptr;
	}

	// 로딩이 완료된 에셋에 본인의 Key, RelativePath 세팅
	pAsset->SetKey(_RelativePath);
	pAsset->SetRelativePath(_RelativePath);

	// 에셋을 맵에 등록
	ASSET_TYPE Type = GetAssetType<T>();
	m_mapAsset[static_cast<UINT>(Type)].insert(make_pair(_RelativePath, pAsset));

	m_bAssetChanged = true;

	return static_cast<T*>(pAsset.Get());
}
