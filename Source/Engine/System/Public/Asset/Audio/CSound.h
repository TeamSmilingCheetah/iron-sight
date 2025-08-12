#pragma once
#include "Engine/System/Public/Asset/Base/CAsset.h"

class CSound :
	public CAsset
{
private:
	FMOD::Sound* SoundBuffer; // Sound 버퍼
	list<FMOD::Channel*> ChannelList; // Sound 가 재생되고 있는 채널 리스트
public:
	// _iRoopCount : 0 (무한반복),  _fVolume : 0 ~ 1(Volume), _bOverlap : 소리 중첩 가능여부
	int Play(int InRoopCount, float InVolume, bool InIsOverlap);
	void RemoveChannel(const FMOD::Channel* InTargetChannel);
	void Stop();

	// 0 ~ 1
	void SetVolume(float InVolume, int InChannelIdx);

	const list<FMOD::Channel*>& GetChannel() { return ChannelList; }

	int Load(const wstring& InFilePath) override;
	int Save(const wstring& InFilePath) override { return S_OK; }

	CLONE_DISABLE(CSound);
	CSound(bool InEngineRes = false);
	~CSound() override;
};
