#pragma once
#include "CAsset.h"

class CSound :
    public CAsset
{
    FMOD::Sound* m_Sound; // Sound ����
    list<FMOD::Channel*> m_listChannel; // Sound �� ����ǰ� �ִ� ä�� ����Ʈ


public:
    // _iRoopCount : 0 (���ѹݺ�),  _fVolume : 0 ~ 1(Volume), _bOverlap : �Ҹ� ��ø ���ɿ���
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
