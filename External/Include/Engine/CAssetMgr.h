#pragma once

#include "singleton.h"
#include "assets.h"

#include "CPathMgr.h"

class CAssetMgr :
    public singleton<CAssetMgr>
{
    SINGLE(CAssetMgr);
private:
    map<wstring, Ptr<CAsset>>   m_mapAsset[(UINT)ASSET_TYPE::END];
    bool                        m_bAssetChanged;

public:
    void Init();

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
    template<typename T>
    int AddAsset(const wstring& _Key, Ptr<T> _Asset);

    template<typename T>
    Ptr<T> FindAsset(const wstring& _Key);

    template<typename T>
    Ptr<T> Load(const wstring& _Key, const wstring& _RelativePath);        
    template<>
    Ptr<CComputeShader> Load(const wstring& _Key, const wstring& _RelativePath)
    {
        return FindAsset<CComputeShader>(_Key).Get();
    }

    Ptr<CTexture> CreateTexture(const wstring& _Key, UINT _Width, UINT _Height, DXGI_FORMAT _PixelFormat, UINT _BindFlag, D3D11_USAGE _Usage = D3D11_USAGE_DEFAULT);
    Ptr<CTexture> CreateTexture(const wstring& _Key, ComPtr<ID3D11Texture2D> _Tex2D);
    
    Ptr<CMeshData> LoadFBX(const wstring& _strPath);
    
    void GetAssetNames(ASSET_TYPE _Type, vector<wstring>& _vecAssetNames);
    const map<wstring, Ptr<CAsset>>& GetAssets(ASSET_TYPE _Type) { return m_mapAsset[(UINT)_Type]; }

    void DeleteAsset(ASSET_TYPE _Type, const wstring& _Key);
};

template<typename T1, typename T2>
constexpr bool IsTypeSame = false;
template<typename T>
constexpr bool IsTypeSame<T, T> = true;

template<typename T>
ASSET_TYPE GetAssetType()
{
    if constexpr (std::is_same_v<T, CMesh>)
        return ASSET_TYPE::MESH;
    if constexpr (std::is_same_v<T, CGraphicShader>)
        return ASSET_TYPE::GRAPHIC_SHADER;    
    if constexpr (IsTypeSame<T, CComputeShader>)
        return ASSET_TYPE::COMPUTE_SHADER;
    if constexpr (IsTypeSame<T, CMeshData>)
        return ASSET_TYPE::MESH_DATA;
    if constexpr (IsTypeSame<T, CMaterial>)
        return ASSET_TYPE::MATERIAL;
    if constexpr (IsTypeSame<T, CPrefab>)
        return ASSET_TYPE::PREFAB;
    if constexpr (IsTypeSame<T, CSound>)
        return ASSET_TYPE::SOUND;
    if constexpr (IsTypeSame<T, CFlipbook>)
        return ASSET_TYPE::FLIPBOOK;
    if constexpr (IsTypeSame<T, CSprite>)
        return ASSET_TYPE::SPRITE;
    if constexpr (IsTypeSame<T, CTexture>)
        return ASSET_TYPE::TEXTURE;
 
   
    
    return ASSET_TYPE::END;
}

template<typename T>
int CAssetMgr::AddAsset(const wstring& _Key, Ptr<T> _Asset)
{
    ASSET_TYPE Type = GetAssetType<T>();

    _Asset->SetKey(_Key);
    m_mapAsset[(UINT)Type].insert(make_pair(_Key, _Asset.Get()));

    m_bAssetChanged = true;

    return S_OK;
}

template<typename T>
inline Ptr<T> CAssetMgr::FindAsset(const wstring& _Key)
{
    ASSET_TYPE Type = GetAssetType<T>();

    map<wstring, Ptr<CAsset>>::iterator iter = m_mapAsset[(UINT)Type].find(_Key);

    if (iter == m_mapAsset[(UINT)Type].end())
        return nullptr;

    return (T*)iter->second.Get();
}

template<typename T>
inline Ptr<T> CAssetMgr::Load(const wstring& _Key, const wstring& _RelativePath)
{
    Ptr<CAsset> pAsset = FindAsset<T>(_Key).Get();

    if (nullptr != pAsset)
        return (T*)pAsset.Get();

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
    m_mapAsset[(UINT)Type].insert(make_pair(_Key, pAsset));

    m_bAssetChanged = true;

    return (T*)pAsset.Get();
}

