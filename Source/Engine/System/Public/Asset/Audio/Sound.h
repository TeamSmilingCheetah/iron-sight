#pragma once
#include "Engine/System/Public/Asset/Base/Asset.h"

/**
 * @brief 사운드 Asset
 *
 * @param SoundBuffer Sound 버퍼
 * @param ChannelList Sound가 재생되고 있는 채널 리스트
 */
class FSound :
	public FAsset
{
private:
	FMOD::Sound* SoundBuffer;
	list<FMOD::Channel*> ChannelList;

public:

	int Play(int InRoopCount, float InVolume, bool InIsOverlap);
	void RemoveChannel(const FMOD::Channel* InTargetChannel);
	void Stop();

	void SetVolume(float InVolume, int InChannelIdx);

	const list<FMOD::Channel*>& GetChannel() { return ChannelList; }

	int Load(const wstring& InFilePath) override;
	int Save(const wstring& InFilePath) override { return S_OK; }

	// Special Member Function
	CLONE_DISABLE(FSound);
	FSound(bool InEngineRes = false);
	~FSound() override;
};
