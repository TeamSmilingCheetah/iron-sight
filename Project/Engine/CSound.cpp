#include "pch.h"
#include "CSound.h"

#include "CEngine.h"
#include "CAssetMgr.h"

FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol,
                             FMOD_CHANNELCONTROL_TYPE controltype
                             , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
                             , void* commanddata1, void* commanddata2);

CSound::CSound(bool _EngineRes)
    : CAsset(SOUND, _EngineRes)
{
}

CSound::~CSound()
{
    if (nullptr != m_Sound)
    {
        FMOD_RESULT result = m_Sound->release();
        m_Sound = nullptr;
    }
}

int CSound::Play(int _iRoopCount, float _fVolume, bool _bOverlap)
{
    if (_iRoopCount <= -1)
    {
        assert(nullptr);
    }

    // 재생되고 있는 채널이 있는데, 중복재생을 허용하지 않았다 -> 재생 안함
    if (!_bOverlap && !m_listChannel.empty())
    {
        return E_FAIL;
    }

    _iRoopCount -= 1;

    FMOD::Channel* pChannel = nullptr;
    CEngine::GetInst()->GetFMODSystem()->playSound(m_Sound, nullptr, false, &pChannel);

    // 재생 실패
    if (nullptr == pChannel)
        return E_FAIL;

    pChannel->setVolume(_fVolume);

    pChannel->setCallback(&CHANNEL_CALLBACK);
    pChannel->setUserData(this);

    pChannel->setMode(FMOD_LOOP_NORMAL);
    pChannel->setLoopCount(_iRoopCount);

    m_listChannel.push_back(pChannel);

    int iIdx = -1;
    pChannel->getIndex(&iIdx);

    return iIdx;
}

void CSound::Stop()
{
    list<FMOD::Channel*>::iterator iter;

    while (!m_listChannel.empty())
    {
        iter = m_listChannel.begin();
        (*iter)->stop();
    }
}

void CSound::SetVolume(float _f, int _iChannelIdx)
{
    auto iter = m_listChannel.begin();

    int iIdx = -1;
    for (; iter != m_listChannel.end(); ++iter)
    {
        (*iter)->getIndex(&iIdx);
        if (_iChannelIdx == iIdx)
        {
            (*iter)->setVolume(_f);
            return;
        }
    }
}

void CSound::RemoveChannel(FMOD::Channel* _pTargetChannel)
{
    auto iter = m_listChannel.begin();
    for (; iter != m_listChannel.end(); ++iter)
    {
        if (*iter == _pTargetChannel)
        {
            m_listChannel.erase(iter);
            return;
        }
    }
}

int CSound::Load(const wstring& _FilePath)
{
    string path(_FilePath.begin(), _FilePath.end());

    if (FMOD_OK != CEngine::GetInst()->GetFMODSystem()->createSound(
        path.c_str(), FMOD_DEFAULT, nullptr, &m_Sound))
    {
        assert(nullptr);
    }

    return S_OK;
}

// =========
// Call Back
// =========
FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol,
                             FMOD_CHANNELCONTROL_TYPE controltype
                             , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
                             , void* commanddata1, void* commanddata2)
{
    auto cppchannel = (FMOD::Channel*)channelcontrol;
    CSound* pSound = nullptr;

    switch (controltype)
    {
    case FMOD_CHANNELCONTROL_CALLBACK_END:
        {
            cppchannel->getUserData((void**)&pSound);
            pSound->RemoveChannel(cppchannel);
        }
        break;
    }

    return FMOD_OK;
}
