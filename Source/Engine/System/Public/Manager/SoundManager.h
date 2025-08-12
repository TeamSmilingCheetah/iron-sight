#pragma once

class FSound;

class FSoundManager :
	public singleton<FSoundManager>
{
	SINGLE(FSoundManager)

private:
	FMOD::System* FMODSystem;

	Ptr<FSound> GameBGM;
	FMOD::DSP* GlobalLowPassDSP;

	Vec3 PlayerPosition;
	Vec3 PlayerForward;
	Vec3 PlayerUp;

	bool bMuffling;
	float MuffleDuration;
	float MuffleElapsed;
	float StartCutoff; // 효과 시작 시 컷오프
	float TargetCutoff; // 회복 완료 시 컷오프

	map<int, Sound3DIdx> Sound3Ds; // ID로 3D 사운드 소스 관리
	int Next3DSoundID;

public:
	void Init();
	void Shutdown();
	void Tick();

	void PlayGameBGM(int InRoop, float InVolume, bool InOverlap = true);
	void StopGameBGM();
	void SetGameBGM(Ptr<FSound> InBGM, bool InStopPrevBGM);
	Ptr<FSound> GetGameBGM() { return GameBGM; }

	int Play3DSound(Ptr<FSound> InSound, const Vec3& InPosition, float InMinDist = 1.0f, float InMaxDist = 10000.0f,
	                int InRoopCount = 0, float InVolume = 1.0f, bool InOverlap = true, bool InInputOverlap = true,
	                int InInputSoundID = -1);

	void Update3DSoundPosition(int soundID, const Vec3& newPosition);
	void UpdateListener(const Vec3& position, const Vec3& forward, const Vec3& up);
	void Stop3DSound(int soundID);
	void StopAll3DSounds();
	void ApplyMuffle(float _fCutoff, float _fDuration);

	FMOD::System* GetFMODSystem() { return FMODSystem; }
};
