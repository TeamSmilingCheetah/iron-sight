#pragma once
#include "Engine/Core/Public/CEntity.h"

class CAsset :
    public CEntity
{
    wstring m_Key;
    wstring m_RelativePath;
    const ASSET_TYPE m_Type;
    int m_RefCount;
    bool m_EngineRes;

public:
    const wstring& GetKey() { return m_Key; }
    const wstring& GetRelativePath() { return m_RelativePath; }

    void SetKey(const wstring& _Key) { m_Key = _Key; }
    void SetRelativePath(const wstring& _Path) { m_RelativePath = _Path; }

    ASSET_TYPE GetAssetType() { return m_Type; }
    int GetRefCount() { return m_RefCount; }

    bool IsEngineAsset() { return m_EngineRes; }

private:
    virtual int Load(const wstring& _strFilePath) = 0;
    virtual int Save(const wstring& _strFilePath) = 0;

    void AddRef() { ++m_RefCount; }

    void Release()
    {
        --m_RefCount;

        if (m_RefCount <= 0)
        {
            delete this;
        }
    }

public:
    CAsset* Clone() override = 0;

    CAsset(ASSET_TYPE _Type, bool _bEngine);
    CAsset(const CAsset& _Origin);
    ~CAsset() override;

    template <typename T>
    friend class Ptr;

    friend class CAssetMgr;
    friend class CFBXLoader;
};
