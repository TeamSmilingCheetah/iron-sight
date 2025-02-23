#pragma once
#include "CAsset.h"

#include "CTexture.h"

class CSprite :
    public CAsset
{
private:
    Ptr<CTexture>       m_Atlas;
    Vec2                m_LeftTopUV;
    Vec2                m_SliceUV;

    Vec2                m_BackgroundUV;
    Vec2                m_OffsetUV;

public:
    void SetAtlasTexture(Ptr<CTexture> _Tex) { m_Atlas = _Tex; }
    void SetLeftTop(Vec2 _LeftTopPixel);
    void SetSlice(Vec2 _SlicePixel);
    void SetBackground(Vec2 _BackgroundPixel);
    void SetOffset(Vec2 _OffsetPixel);

    void Binding();
    static void Clear();

private:
    virtual int Load(const wstring& _FilePath) override;

public:
    virtual int Save(const wstring& _strFilePath) override;

public:
    CLONE_DISABLE(CSprite);
    CSprite(bool _bEngineRes = false);
    ~CSprite();
};
