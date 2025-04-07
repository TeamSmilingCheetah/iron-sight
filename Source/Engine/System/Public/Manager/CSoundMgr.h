#pragma once

class CSound;


struct Sound3DIdx
{
	Ptr<CSound> sound;
	FMOD::Channel* channel;
	Vec3 position;
	int channelIdx;
	float minDist;
	float maxDist;
};

class CSoundMgr :
	public singleton<CSoundMgr>
{
	SINGLE(CSoundMgr)

private:
	Ptr<CSound> m_GameBGM;

	Vec3 m_PlayerPosition;
	Vec3 m_PlayerForward;
	Vec3 m_PlayerUp;

	map<int, Sound3DIdx> m_3DSounds; // ID로 3D 사운드 소스 관리
	int m_next3DSoundID;

public:
	void PlayGameBGM(int _Roop, float _fVolume, bool _Overlap = true);
	void StopGameBGM();
	void SetGameBGM(Ptr<CSound> _BGM, bool _StopprevBGM);
	Ptr<CSound> GetGameBGM() { return m_GameBGM; }

	// sound : 재생할 사운드, pos : 재생될 위치, minDist : 최소 거리, maxDist : 최대 거리, _iRoopCount : 0 (무한반복),  Volume : 0 ~ 1(Volume), overlap : 소리 중첩 가능여부, inputoverlap : 이미 등록된 id를 다시 재생 _inputSoundID : -1이면 평소대로, 그 외이면 id검색해서 중복되면 추가처리
	int Play3DSound(Ptr<CSound> sound, const Vec3& pos, float minDist = 1.0f, float maxDist = 10000.0f, int _iRoopCount = 0, float volume = 1.0f, bool overlap = true, bool inputoverlap = true, int _inputSoundID = -1);

	void Update3DSoundPosition(int soundID, const Vec3& newPosition);
	void UpdateListener(const Vec3& position, const Vec3& forward, const Vec3& up);
	void Stop3DSound(int soundID);
	void StopAll3DSounds();


	void Tick();

};

