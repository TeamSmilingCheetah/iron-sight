#pragma once
#include "Engine/System/Public/Asset/Base/CAsset.h"
#include "Engine/System/Public/Asset/Texture/CSprite.h"

class CFlipbook :
    public CAsset
{
    vector<Ptr<CSprite>> m_vecSprite;

public:
    void AddSprite(Ptr<CSprite> _Sprite) { m_vecSprite.push_back(_Sprite); }
    UINT GetMaxSprite() { return static_cast<UINT>(m_vecSprite.size()); }
    Ptr<CSprite> GetSprite(int _Idx) { return m_vecSprite[_Idx]; }

private:
    int Load(const wstring& _FilePath) override;

public:
    int Save(const wstring& _strFilePath) override;

    CLONE_DISABLE(CFlipbook);
    CFlipbook(bool _bEngineRes = false);
    ~CFlipbook() override;
};
