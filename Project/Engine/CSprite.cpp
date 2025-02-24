#include "pch.h"
#include "CSprite.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CTexture.h"

#include "CPathMgr.h"

CSprite::CSprite(bool _bEngineRes)
    : CAsset(SPRITE, _bEngineRes)
{
}

CSprite::~CSprite()
{
}

void CSprite::SetLeftTop(Vec2 _LeftTopPixel)
{
    assert(m_Atlas.Get());

    m_LeftTopUV = Vec2(_LeftTopPixel.x / m_Atlas->GetWidth(),
                       _LeftTopPixel.y / m_Atlas->GetHeight());
}

void CSprite::SetSlice(Vec2 _SlicePixel)
{
    assert(m_Atlas.Get());

    m_SliceUV = Vec2(_SlicePixel.x / m_Atlas->GetWidth(), _SlicePixel.y / m_Atlas->GetHeight());
}

void CSprite::SetBackground(Vec2 _BackgroundPixel)
{
    assert(m_Atlas.Get());

    m_BackgroundUV = Vec2(_BackgroundPixel.x / m_Atlas->GetWidth(),
                          _BackgroundPixel.y / m_Atlas->GetHeight());
}

void CSprite::SetOffset(Vec2 _OffsetPixel)
{
    assert(m_Atlas.Get());

    m_OffsetUV = Vec2(_OffsetPixel.x / m_Atlas->GetWidth(), _OffsetPixel.y / m_Atlas->GetHeight());
}

void CSprite::Binding()
{
    static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::SPRITE);
    SpriteInfo info = {};

    if (nullptr == m_Atlas)
    {
        Clear();
    }

    else
    {
        m_Atlas->Binding(12);

        info.LeftTopUV = m_LeftTopUV;
        info.SliceUV = m_SliceUV;
        info.BackgroundUV = m_BackgroundUV;
        info.OffsetUV = m_OffsetUV;
        info.SpriteUse = true;

        pCB->SetData(&info);
    }

    pCB->Binding();
}

void CSprite::Clear()
{
    static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::SPRITE);

    SpriteInfo info = {};
    pCB->SetData(&info);
    pCB->Binding();

    CTexture::Clear(12);
}


int CSprite::Save(const wstring& _strFilePath)
{
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _strFilePath.c_str(), L"wb");
    assert(pFile);

    SaveAssetRef(m_Atlas, pFile);

    fwrite(&m_LeftTopUV, sizeof(Vec2), 1, pFile);
    fwrite(&m_SliceUV, sizeof(Vec2), 1, pFile);
    fwrite(&m_BackgroundUV, sizeof(Vec2), 1, pFile);
    fwrite(&m_OffsetUV, sizeof(Vec2), 1, pFile);

    fclose(pFile);

    wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
    int FindIdx = _strFilePath.find(strContentPath);
    if (FindIdx != -1)
    {
        wstring RelativePath = _strFilePath.substr(strContentPath.length(), _strFilePath.length());
        SetRelativePath(RelativePath);
    }

    return S_OK;
}

int CSprite::Load(const wstring& _FilePath)
{
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _FilePath.c_str(), L"rb");
    assert(pFile);

    LoadAssetRef(m_Atlas, pFile);

    fread(&m_LeftTopUV, sizeof(Vec2), 1, pFile);
    fread(&m_SliceUV, sizeof(Vec2), 1, pFile);
    fread(&m_BackgroundUV, sizeof(Vec2), 1, pFile);
    fread(&m_OffsetUV, sizeof(Vec2), 1, pFile);

    fclose(pFile);

    return S_OK;
}
