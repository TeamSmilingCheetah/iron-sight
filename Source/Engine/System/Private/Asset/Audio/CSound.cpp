#include "pch.h"
#include "System/Public/Asset/Audio/CSound.h"

#include "System/Public/Manager/SoundManager.h"

static FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* InChannelControl,
                                    FMOD_CHANNELCONTROL_TYPE InControlType
                                    , FMOD_CHANNELCONTROL_CALLBACK_TYPE InCallBackType
                                    , void* InCommandData1, void* InCommandData2);

CSound::CSound(bool InEngineRes)
	: CAsset(SOUND, InEngineRes)
	  , SoundBuffer(nullptr)
{
}

CSound::~CSound()
{
	const wstring Name = this->GetName();

	if (SoundBuffer)
	{
		SoundBuffer->release();
		SoundBuffer = nullptr;
	}

	LOG_DEBUG_F("[Asset][Sound] {} is Deleted", WStringToString(Name));
}

int CSound::Play(int InRoopCount, float InVolume, bool InIsOverlap)
{
	if (InRoopCount <= -1)
	{
		assert(nullptr);
	}

	// 재생되고 있는 채널이 있는데, 중복재생을 허용하지 않았다 -> 재생 안함
	if (!InIsOverlap && !ChannelList.empty())
	{
		return E_FAIL;
	}

	InRoopCount -= 1;

	FMOD::Channel* Channel = nullptr;
	FSoundManager::GetInst()->GetFMODSystem()->playSound(SoundBuffer, nullptr, false, &Channel);

	// 재생 실패
	if (nullptr == Channel)
	{
		return E_FAIL;
	}

	Channel->setVolume(InVolume);

	Channel->setCallback(&CHANNEL_CALLBACK);
	Channel->setUserData(this);

	Channel->setMode(FMOD_LOOP_NORMAL);
	Channel->setLoopCount(InRoopCount);

	ChannelList.push_back(Channel);

	int iIdx = -1;
	Channel->getIndex(&iIdx);

	return iIdx;
}

void CSound::Stop()
{
	for (auto Channel : ChannelList)
	{
		Channel->stop();
	}

	ChannelList.clear();
}

void CSound::SetVolume(float InVolume, int InChannelIdx)
{
	int iIdx = -1;

	for (auto iter = ChannelList.begin(); iter != ChannelList.end(); ++iter)
	{
		(*iter)->getIndex(&iIdx);
		if (InChannelIdx == iIdx)
		{
			(*iter)->setVolume(InVolume);
			return;
		}
	}
}

void CSound::RemoveChannel(const FMOD::Channel* InTargetChannel)
{
	for (auto iter = ChannelList.begin(); iter != ChannelList.end(); ++iter)
	{
		if (*iter == InTargetChannel)
		{
			ChannelList.erase(iter);
			return;
		}
	}
}

int CSound::Load(const wstring& InFilePath)
{
	string Path = WStringToString(InFilePath);
	auto* System = FSoundManager::GetInst()->GetFMODSystem();

	if (System->createSound(Path.c_str(), FMOD_DEFAULT, nullptr, &SoundBuffer) != FMOD_OK)
	{
		assert(nullptr);
	}

	return S_OK;
}

// =========
// Call Back
// =========
FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* InChannelControl,
                             FMOD_CHANNELCONTROL_TYPE InControlType,
                             FMOD_CHANNELCONTROL_CALLBACK_TYPE InCallBackType,
                             void* InCommandData1, void* InCommandData2)
{
	auto Channel = reinterpret_cast<FMOD::Channel*>(InChannelControl);
	CSound* OwnerSound = nullptr;

	switch (InControlType)
	{
	case FMOD_CHANNELCONTROL_CALLBACK_END:
		{
			Channel->getUserData(reinterpret_cast<void**>(&OwnerSound));
			OwnerSound->RemoveChannel(Channel);
		}
		break;
	default:
		{
			// Do Nothing
		}
	}

	return FMOD_OK;
}
