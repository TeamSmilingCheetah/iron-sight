#include "pch.h"
#include "Engine/System/Public/Manager/SoundManager.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Asset/Audio/CSound.h"

FSoundManager::FSoundManager() = default;

FSoundManager::~FSoundManager()
{
	if (FMODSystem)
	{
		{
			FMODSystem->release();
			FMODSystem = nullptr;
		}
	}

	LOG_INFO("[Engine][SoundManager] Instance Deleted");
}

void FSoundManager::Init()
{
	// FMOD 초기화
	System_Create(&FMODSystem);
	assert(FMODSystem);

	// 32개 채널 생성
	FMODSystem->init(32, FMOD_INIT_NORMAL, nullptr);

	// 3D 환경 설정(도플러 효과, 거리 단위, 롤오프 스케일)
	FMODSystem->set3DSettings(1.0f, 1.0f, 1.0f);

	FMOD_RESULT result = FMODSystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &GlobalLowPassDSP);
	if (result != FMOD_OK || !GlobalLowPassDSP)
	{
		assert(false && "Low-pass DSP 생성 실패");
	}
	// 초기에는 효과가 없도록 컷오프 주파수를 최대로 설정
	GlobalLowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 22050.0f);
}

void FSoundManager::Shutdown()
{
	if (GlobalLowPassDSP)
	{
		GlobalLowPassDSP->release();
		GlobalLowPassDSP = nullptr;
	}

	LOG_INFO("[Engine][SoundManager] Shutdown Complete");
}

/*
 * sound : 재생할 사운드, pos : 재생될 위치, minDist : 최소 거리,
 * maxDist : 최대 거리, _iRoopCount : 0 (무한반복),
 * Volume : 0 ~ 1(Volume), overlap : 소리 중첩 가능여부, inputoverlap : 이미 등록된 id를 다시 재생
 * _inputSoundID : -1이면 평소대로, 그 외이면 id검색해서 중복되면 추가처리
 */
int FSoundManager::Play3DSound(Ptr<CSound> InSound, const Vec3& InPosition, float InMinDist, float InMaxDist, int InRoopCount, float InVolume, bool InOverlap, bool InInputOverlap, int InInputSoundID)
{
	if (nullptr == InSound)
	{
		return -1;
	}

	// 입력된 inputSoundID가 유효하면 해당 ID의 사운드가 이미 재생 중인지 확인
	if (InInputSoundID != -1)
	{
		map<int, Sound3DIdx>::iterator soundit = Sound3Ds.find(InInputSoundID);
		if (soundit != Sound3Ds.end() && nullptr != soundit->second.Channel)
		{
			bool isPlaying = false;
			soundit->second.Channel->isPlaying(&isPlaying);

			if (isPlaying && !InInputOverlap)
			{
				// 이미 재생 중 + 검사용 ID까지존재 + 추가재생x = 플레이 안함
				return InInputSoundID;
			}
			else
			{
				// 기존 사운드 중지 후 map에 삭제(새로 id발급받아서 넘길거임)
				soundit->second.Channel->stop();
				Sound3Ds.erase(soundit);
			}
		}
	}

	// 새로운 3D 사운드 정보 생성
	Sound3DIdx source;
	source.Sound = InSound;
	source.Position = InPosition;
	source.MinDist = InMinDist;
	source.MaxDist = InMaxDist;

	// 사운드 재생
	int channelIdx = InSound->Play(InRoopCount, InVolume, InOverlap);

	if (channelIdx < 0)
	{
		return -1;
	}

	// 채널 가져오기
	FMOD::Channel* channel = nullptr;
	list<FMOD::Channel*>::const_iterator iter = InSound->GetChannel().begin();
	for (; iter != InSound->GetChannel().end(); ++iter)
	{
		int idx = -1;
		(*iter)->getIndex(&idx);

		if (idx == channelIdx)
		{
			channel = *iter;
			break;
		}
	}

	if (nullptr == channel)
	{
		return -1;
	}

	// 채널을 3D 모드로 설정
	channel->setMode(FMOD_3D | FMOD_3D_LINEARROLLOFF);

	// 3D 속성 설정
	FMOD_VECTOR fmodPos = { InPosition.x, InPosition.y, InPosition.z };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f }; // 속도는 0으로 설정

	channel->set3DAttributes(&fmodPos, &vel);
	channel->set3DMinMaxDistance(InMinDist, InMaxDist);

	// 소스 정보 저장
	source.Channel = channel;
	source.ChannelIdx = channelIdx;

	int soundID = Next3DSoundID++;
	Sound3Ds[soundID] = source;

	return soundID;
}

void FSoundManager::UpdateListener(const Vec3& position, const Vec3& forward, const Vec3& up)
{
	PlayerPosition = position;
	PlayerForward = forward;
	PlayerUp = up;

	FMOD_VECTOR fmodPos = { position.x, position.y, position.z };
	FMOD_VECTOR fmodVel = { 0.0f, 0.0f, 0.0f }; // 속도는 0으로 설정
	FMOD_VECTOR fmodForward = { forward.x, forward.y, forward.z };
	FMOD_VECTOR fmodUp = { up.x, up.y, up.z };

	// FMOD에 듵는 사람 위치 설정
	FMODSystem->set3DListenerAttributes(0, &fmodPos, &fmodVel, &fmodForward, &fmodUp);
}

void FSoundManager::Update3DSoundPosition(int soundID, const Vec3& newPosition)
{
	map<int, Sound3DIdx>::iterator iter = Sound3Ds.find(soundID);
	if (iter == Sound3Ds.end() || nullptr == iter->second.Channel)
	{
		return;
	}

	iter->second.Position = newPosition;

	FMOD_VECTOR fmodPos = { newPosition.x, newPosition.y, newPosition.z };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f }; // 속도는 0으로 설정

	iter->second.Channel->set3DAttributes(&fmodPos, &vel);
}

void FSoundManager::Stop3DSound(int soundID)
{
	map<int, Sound3DIdx>::iterator it = Sound3Ds.find(soundID);
	if (it == Sound3Ds.end() || nullptr == it->second.Channel)
	{
		return;
	}

	it->second.Channel->stop();
	Sound3Ds.erase(it);
}

void FSoundManager::StopAll3DSounds()
{
	for (auto& pair : Sound3Ds) {
		if (nullptr != pair.second.Channel)
		{
			pair.second.Channel->stop();
		}
	}
	Sound3Ds.clear();
}

void FSoundManager::ApplyMuffle(float _fCutoff, float _fDuration)
{
	// 1) DSP 걸기
	FMOD::ChannelGroup* masterGroup = nullptr;
	FMODSystem->getMasterChannelGroup(&masterGroup);
	masterGroup->addDSP(0, GlobalLowPassDSP);

	// 2) 상태 초기화
	bMuffling = true;
	MuffleDuration = _fDuration;
	MuffleElapsed = 0.0f;
	StartCutoff = _fCutoff;             // 먹먹함 시작 컷오프 (작은 값)
	TargetCutoff = 22050.0f;            // 회복될 때 컷오프 (기본값)

	// 3) 즉시 시작 컷오프로 세팅
	GlobalLowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, StartCutoff);
}

void FSoundManager::Tick()
{
	// 이미 재생이 끝난 3D 사운드 채널 제거
	map<int, Sound3DIdx>::iterator it = Sound3Ds.begin();
	while (it != Sound3Ds.end())
	{
		bool isPlaying = false;
		//해당 채널이 비어있는게 아니라면 플레이중인지 확인
		if (nullptr != it->second.Channel)
		{
			it->second.Channel->isPlaying(&isPlaying);
		}

		// 플레이중이 아니면 제거
		if (!isPlaying)
		{
			it = Sound3Ds.erase(it);
		}
		else
		{
			++it;
		}
	}

	// FMOD 업데이트, 3D 사운드 거리 감쇠 계산을 위해 필요
	FMODSystem->update();

	if (!bMuffling)
	{
		return;
	}

	// 경과 시간 누적
	MuffleElapsed += DT;
	float t = min(MuffleElapsed / MuffleDuration, 1.0f);

	// 보간 계산: 컷오프 값
	float currentCutoff = StartCutoff + (TargetCutoff - StartCutoff) * t;
	GlobalLowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, currentCutoff);

	// 완료된 경우 DSP 제거
	if (t >= 1.0f)
	{
		FMOD::ChannelGroup* masterGroup = nullptr;
		FMODSystem->getMasterChannelGroup(&masterGroup);
		masterGroup->removeDSP(GlobalLowPassDSP);

		bMuffling = false;
	}
}

void FSoundManager::PlayGameBGM(int InRoop, float InVolume, bool InOverlap)
{
	if (nullptr != GameBGM)
	{
		GameBGM->Play(InRoop, InVolume, InOverlap);
	}
}

void FSoundManager::StopGameBGM()
{
	if (nullptr != GameBGM)
	{
		GameBGM->Stop();
	}
}

void FSoundManager::SetGameBGM(Ptr<CSound> InBGM, bool InStopPrevBGM)
{
	if (InStopPrevBGM && nullptr != GameBGM)
	{
		GameBGM->Stop();
	}

	GameBGM = InBGM;
}
