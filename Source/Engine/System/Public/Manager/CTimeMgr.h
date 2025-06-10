#pragma once

/**
 * @brief 프레임마다 일정한 시간 간격을 계산하여 동일한 동작을 보장하도록 하는 매니저 클래스
 *
 * @param m_Frequency 1초당 카운팅 가능 수
 * @param m_CurrentCount 현재 카운팅
 * @param m_PrevCount 이전 프레임 카운팅
 * @param m_fDT 1프레임에 걸린 시간 (DeltaTime)
 * @param m_Time 누적 시간
 * @param m_fEngineDT
 * @param m_fEngineTime
 * @param m_Second
 * @param m_FPS
 * @param m_TotalAverageFPS TimeMgr 생성 이후의 전체 평균 FPS
 * @param m_IsStop
 * @param m_TimeInfo
 */
class CTimeMgr
	: public singleton<CTimeMgr>
{
	SINGLE(CTimeMgr);
	LARGE_INTEGER m_Frequency;
	LARGE_INTEGER m_CurrentCount;
	LARGE_INTEGER m_PrevCount;

	float m_fDT;
	float m_Time;

	float m_fEngineDT;
	float m_fEngineTime;

	double m_Second;

	UINT m_FPS;
	float m_TotalAverageFPS;

	bool m_IsStop;

	wchar_t m_TimeInfo[255];

public:
	void Init();
	void Tick();

	void SetStopMode(bool PIsStop);
	float GetDeltaTime() const { return m_fDT; }
	float GetEngineDeltaTime() const { return m_fEngineDT; }
};
