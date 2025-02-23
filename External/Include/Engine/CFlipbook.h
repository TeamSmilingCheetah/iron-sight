#pragma once
#include "CAsset.h"

#include "CSprite.h"

class CFlipbook :
    public CAsset
{
private:
    vector<Ptr<CSprite>>    m_vecSprite;

public:
    void AddSprite(Ptr<CSprite> _Sprite) { m_vecSprite.push_back(_Sprite); }
    UINT GetMaxSprite() { return (UINT)m_vecSprite.size(); }
    Ptr<CSprite> GetSprite(int _Idx) { return m_vecSprite[_Idx]; }

private:
    virtual int Load(const wstring& _FilePath) override;

public:
    virtual int Save(const wstring& _strFilePath) override;

public:
    CLONE_DISABLE(CFlipbook);
    CFlipbook(bool _bEngineRes = false);
    ~CFlipbook();
};

