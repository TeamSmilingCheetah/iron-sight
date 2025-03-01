#pragma once
#include "System/Public/Asset/Base/CAsset.h"

class CSound :
    public CAsset
{
    FMOD::Sound* m_Sound; // Sound 버퍼
    list<FMOD::Channel*> m_listChannel; // Sound 가 재생되고 있는 채널 리스트


public:
    // _iRoopCount : 0 (무한반복),  _fVolume : 0 ~ 1(Volume), _bOverlap : 소리 중첩 가능여부
    int Play(int _iRoopCount, float _fVolume, bool _bOverlap);
    void RemoveChannel(FMOD::Channel* _pTargetChannel);
    void Stop();

    // 0 ~ 1
    void SetVolume(float _f, int _iChannelIdx);

    int Load(const wstring& _FilePath) override;
    int Save(const wstring& _FilePath) override { return S_OK; }

    CLONE_DISABLE(CSound);
    CSound(bool _EngineRes = false);
    ~CSound() override;
};
