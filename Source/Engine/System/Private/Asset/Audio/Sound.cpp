#include "pch.h"
#include "Engine/System/Public/Asset/Audio/Sound.h"

#include "Engine/System/Public/Manager/SoundManager.h"

static FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* InChannelControl,
                                    FMOD_CHANNELCONTROL_TYPE InControlType
                                    , FMOD_CHANNELCONTROL_CALLBACK_TYPE InCallBackType
                                    , void* InCommandData1, void* InCommandData2);

FSound::FSound(bool InEngineRes)
	: FAsset(SOUND, InEngineRes)
	  , SoundBuffer(nullptr)
{
}

FSound::~FSound()
{
	// const wstring Name = this->GetName();

	if (SoundBuffer)
	{
		SoundBuffer->release();
		SoundBuffer = nullptr;
	}

	// 어차피 Sound 소멸자는 Log Manager 종료 시기보다 뒤에 호출된다
	// LOG_DEBUG_F("[Asset][Sound] {} is Deleted", WStringToString(Name));
}

/**
 * @brief 소리 출력 함수
 *
 * @param InRoopCount 반복 횟수 (0이라면 무한 반복)
 * @param InVolume 0 ~ 1 사이의 Volume 수치
 * @param InIsOverlap 소리 중첩 가능 여부
 * @return Channel Index
 */
int FSound::Play(int InRoopCount, float InVolume, bool InIsOverlap)
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

	int ChannelIndex = -1;
	Channel->getIndex(&ChannelIndex);

	return ChannelIndex;
}

void FSound::Stop()
{
	for (auto Channel : ChannelList)
	{
		Channel->stop();
	}

	ChannelList.clear();
}

/**
 * @brief 볼륨 설정 함수
 *
 * @param InVolume 0 ~ 1 사이의 Volume 값
 * @param InChannelIdx Volume을 설정할 Channel Index
 */
void FSound::SetVolume(float InVolume, int InChannelIdx)
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

void FSound::RemoveChannel(const FMOD::Channel* InTargetChannel)
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

int FSound::Load(const wstring& InFilePath)
{
	string Path = WStringToString(InFilePath);
	auto* System = FSoundManager::GetInst()->GetFMODSystem();

	if (System->createSound(Path.c_str(), FMOD_DEFAULT, nullptr, &SoundBuffer) != FMOD_OK)
	{
		assert(nullptr);
	}

	return S_OK;
}

/**
 * @brief FMOD Channel Event Call Back Function
 * FMOD::Channel 객체에 등록되어 사운드 재생, 종료와 같은 다양한 채널 관련 이벤트에 대한 알림을 수신하는 콜백 함수
 * @param InChannelControl 콜백을 트리거한 FMOD_CHANNELCONTROL 객체에 대한 포인터
 * @param InControlType 콜백을 트리거한 채널 Control Type (현재는 FMOD_CHANNELCONTROL_CALLBACK_END만 처리 중)
 * @param InCallBackType Event Type
 * @param InCommandData1 Event Data Pointer 1
 * @param InCommandData2 Event Data Pointer 2
 * @return [Success] FMOD_OK | [Fail] FMOD Error Code
 */
FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* InChannelControl,
                             FMOD_CHANNELCONTROL_TYPE InControlType,
                             FMOD_CHANNELCONTROL_CALLBACK_TYPE InCallBackType,
                             void* InCommandData1, void* InCommandData2)
{
	auto Channel = reinterpret_cast<FMOD::Channel*>(InChannelControl);
	FSound* OwnerSound = nullptr;

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
