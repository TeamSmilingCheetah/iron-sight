#include "pch.h"
#include "CFlipbookPlayer.h"

#include "CTimeMgr.h"

CFlipbookPlayer::CFlipbookPlayer()
    : CComponent(COMPONENT_TYPE::FLIPBOOKPLAYER)
      , m_SpriteIdx(0)
      , m_Repeat(false)
      , m_FPS(24)
      , m_Time(0.f)
{
}


CFlipbookPlayer::~CFlipbookPlayer()
{
}

void CFlipbookPlayer::FinalTick()
{
    if (nullptr == m_CurFlipbook || (m_Finish && !m_Repeat))
        return;

    if (m_Finish && m_Repeat)
    {
        m_SpriteIdx = 0;
        m_Finish = false;
    }

    // 스프라이트 프레임 간격 시간
    float Term = (1.f / m_FPS);

    if (Term < m_Time)
    {
        ++m_SpriteIdx;

        // Sprite 인덱스 초과
        if (m_CurFlipbook->GetMaxSprite() <= m_SpriteIdx)
        {
            m_SpriteIdx -= 1;
            m_Finish = true;
        }

        m_Time -= Term;
    }

    m_Time += DT;
}


void CFlipbookPlayer::AddFlipbook(int _idx, Ptr<CFlipbook> _Flipbook)
{
    if (m_vecFlipbook.size() <= _idx)
    {
        m_vecFlipbook.resize(_idx + 1);
    }

    m_vecFlipbook[_idx] = _Flipbook;
}

void CFlipbookPlayer::Binding()
{
    if (nullptr == m_CurFlipbook)
        return;

    Ptr<CSprite> Sprite = m_CurFlipbook->GetSprite(m_SpriteIdx);

    if (nullptr != Sprite)
    {
        Sprite->Binding();
    }
}

void CFlipbookPlayer::Clear()
{
    if (nullptr == m_CurFlipbook)
        return;

    CSprite::Clear();
}


void CFlipbookPlayer::SaveComponent(FILE* _File)
{
    fwrite(&m_SpriteIdx, sizeof(int), 1, _File);
    fwrite(&m_Repeat, sizeof(bool), 1, _File);
    fwrite(&m_FPS, sizeof(float), 1, _File);

    size_t count = m_vecFlipbook.size();
    fwrite(&count, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < m_vecFlipbook.size(); ++i)
    {
        SaveAssetRef(m_vecFlipbook[i], _File);
    }

    SaveAssetRef(m_CurFlipbook, _File);
}

void CFlipbookPlayer::LoadComponent(FILE* _File)
{
    fread(&m_SpriteIdx, sizeof(int), 1, _File);
    fread(&m_Repeat, sizeof(bool), 1, _File);
    fread(&m_FPS, sizeof(float), 1, _File);

    size_t count = 0;
    fread(&count, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < count; ++i)
    {
        Ptr<CFlipbook> pFlipBook;
        LoadAssetRef(pFlipBook, _File);
        m_vecFlipbook.push_back(pFlipBook);
    }

    LoadAssetRef(m_CurFlipbook, _File);
}
