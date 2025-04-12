#include "pch.h"
#include "System/Public/Manager/CSoundMgr.h"
#include "System/Public/Asset/Audio/CSound.h"
#include "Core/Public/CEngine.h"

CSoundMgr::CSoundMgr()
{

}

CSoundMgr::~CSoundMgr()
{

}

int CSoundMgr::Play3DSound(Ptr<CSound> sound, const Vec3& pos, float minDist, float maxDist, int _iRoopCount, float volume, bool overlap, bool inputoverlap, int _inputSoundID)
{
	if (nullptr == sound)
		return -1;

	// 입력된 inputSoundID가 유효하면 해당 ID의 사운드가 이미 재생 중인지 확인
	if (_inputSoundID != -1)
	{
		map<int, Sound3DIdx>::iterator soundit = m_3DSounds.find(_inputSoundID);
		if (soundit != m_3DSounds.end() && nullptr != soundit->second.channel)
		{
			bool isPlaying = false;
			soundit->second.channel->isPlaying(&isPlaying);

			if (isPlaying && !inputoverlap)
			{
				// 이미 재생 중 + 검사용 ID까지존재 + 추가재생x = 플레이 안함
				return _inputSoundID;
			}
			else
			{
				// 기존 사운드 중지 후 map에 삭제(새로 id발급받아서 넘길거임)
				soundit->second.channel->stop();
				m_3DSounds.erase(soundit);
			}
		}
	}

	// 새로운 3D 사운드 정보 생성
	Sound3DIdx source;
	source.sound = sound;
	source.position = pos;
	source.minDist = minDist;
	source.maxDist = maxDist;

	// 사운드 재생
	int channelIdx = sound->Play(_iRoopCount, volume, overlap);

	if (channelIdx < 0)
		return -1;

	// 채널 가져오기
	FMOD::Channel* channel = nullptr;
	list<FMOD::Channel*>::const_iterator iter = sound->GetChannel().begin();
	for (; iter != sound->GetChannel().end(); ++iter)
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
		return -1;

	// 채널을 3D 모드로 설정
	channel->setMode(FMOD_3D | FMOD_3D_LINEARROLLOFF);

	// 3D 속성 설정
	FMOD_VECTOR fmodPos = { pos.x, pos.y, pos.z };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f }; // 속도는 0으로 설정

	channel->set3DAttributes(&fmodPos, &vel);
	channel->set3DMinMaxDistance(minDist, maxDist);

	// 소스 정보 저장
	source.channel = channel;
	source.channelIdx = channelIdx;

	int soundID = m_next3DSoundID++;
	m_3DSounds[soundID] = source;

	return soundID;
}

void CSoundMgr::UpdateListener(const Vec3& position, const Vec3& forward, const Vec3& up)
{
	m_PlayerPosition = position;
	m_PlayerForward = forward;
	m_PlayerUp = up;

	FMOD_VECTOR fmodPos = { position.x, position.y, position.z };
	FMOD_VECTOR fmodVel = { 0.0f, 0.0f, 0.0f }; // 속도는 0으로 설정
	FMOD_VECTOR fmodForward = { forward.x, forward.y, forward.z };
	FMOD_VECTOR fmodUp = { up.x, up.y, up.z };

	// FMOD에 듵는 사람 위치 설정
	CEngine::GetInst()->GetFMODSystem()->set3DListenerAttributes(0, &fmodPos, &fmodVel, &fmodForward, &fmodUp);
}

void CSoundMgr::Update3DSoundPosition(int soundID, const Vec3& newPosition)
{
	map<int, Sound3DIdx>::iterator iter = m_3DSounds.find(soundID);
	if (iter == m_3DSounds.end() || nullptr == iter->second.channel)
		return;

	iter->second.position = newPosition;

	FMOD_VECTOR fmodPos = { newPosition.x, newPosition.y, newPosition.z };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f }; // 속도는 0으로 설정

	iter->second.channel->set3DAttributes(&fmodPos, &vel);
}

void CSoundMgr::Stop3DSound(int soundID)
{
	map<int, Sound3DIdx>::iterator it = m_3DSounds.find(soundID);
	if (it == m_3DSounds.end() || nullptr == it->second.channel)
		return;

	it->second.channel->stop();
	m_3DSounds.erase(it);
}

void CSoundMgr::StopAll3DSounds()
{
	for (auto& pair : m_3DSounds) {
		if (nullptr != pair.second.channel)
			pair.second.channel->stop();
	}
	m_3DSounds.clear();
}

void CSoundMgr::Tick()
{
	// 이미 재생이 끝난 3D 사운드 채널 제거
	map<int, Sound3DIdx>::iterator it = m_3DSounds.begin();
	while (it != m_3DSounds.end())
	{
		bool isPlaying = false;
		//해당 채널이 비어있는게 아니라면 플레이중인지 확인
		if (nullptr != it->second.channel)
			it->second.channel->isPlaying(&isPlaying);

		// 플레이중이 아니면 제거
		if (!isPlaying)
			it = m_3DSounds.erase(it);
		else
			++it;
	}

	// FMOD 업데이트, 3D 사운드 거리 감쇠 계산을 위해 필요
	CEngine::GetInst()->GetFMODSystem()->update();
}

void CSoundMgr::PlayGameBGM(int _Roop, float _fVolume, bool _Overlap)
{
	if (nullptr != m_GameBGM)
	{
		m_GameBGM->Play(_Roop, _fVolume, _Overlap);
	}
}

void CSoundMgr::StopGameBGM()
{
	if (nullptr != m_GameBGM)
	{
		m_GameBGM->Stop();
	}
}

void CSoundMgr::SetGameBGM(Ptr<CSound> _BGM, bool _StopprevBGM)
{
	if (_StopprevBGM && nullptr != m_GameBGM)
		m_GameBGM->Stop();

	m_GameBGM = _BGM;
}
