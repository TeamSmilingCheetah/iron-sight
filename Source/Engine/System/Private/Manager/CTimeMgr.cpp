#include "pch.h"
#include "System/Public/Manager/CTimeMgr.h"
#include "System/Public/Manager/CFontMgr.h"

CTimeMgr::CTimeMgr()
	: m_Frequency{}
	  , m_CurrentCount{}
	  , m_PrevCount{}
	  , m_fDT(0.f)
	  , m_Time(0.f)
	  , m_fEngineDT(0.f)
	  , m_fEngineTime(0.f)
	  , m_Second(0.)
	  , m_FPS(0)
	  , m_TotalAverageFPS(0)
	  , m_IsStop(true)
	  , m_TimeInfo{}
{
}

CTimeMgr::~CTimeMgr() = default;

void CTimeMgr::Init()
{
	QueryPerformanceFrequency(&m_Frequency);
	QueryPerformanceCounter(&m_PrevCount);
}

/**
 * @brief 엔진 전체 FPS 연산 및 시간 측정
 */
void CTimeMgr::Tick()
{
	QueryPerformanceCounter(&m_CurrentCount);

	// Tick 진행에 따른 시간 누적
	m_fEngineDT = static_cast<float>(m_CurrentCount.QuadPart - m_PrevCount.QuadPart) / static_cast<float>(m_Frequency.
		QuadPart);
	m_fEngineTime += m_fEngineDT;
	m_Second += m_fEngineDT;

	// 10개 샘플링 후 평균 계산값을 출력하여 통계적으로 유의한 값 제공
	constexpr int SampleCount = 10;
	static float FPSSamples[SampleCount] = {0.f};
	static int CurrentSample = 0;

	// 순환 버퍼에 FPS 정보 입력
	FPSSamples[CurrentSample] = 1.f / m_fEngineDT;
	CurrentSample = (CurrentSample + 1) % SampleCount;

	// 평균 FPS 연산
	// TODO(KHJ): Overflow 발생에 유의할 것
	m_TotalAverageFPS = (m_TotalAverageFPS * (m_fEngineTime - m_fEngineDT) + 1) / m_fEngineTime;

	// 평균 연산
	float AverageFPS = 0.f;
	for (int i = 0; i < SampleCount; ++i)
	{
		AverageFPS += FPSSamples[i];
	}
	AverageFPS /= SampleCount;

	// 0.5초마다 FPS 업데이트
	if (m_Second >= 0.5f)
	{
		m_FPS = static_cast<int>(AverageFPS);
		m_Second = 0.f;

		// 프레임 관련 정보 제공
		swprintf_s(m_TimeInfo, L"FPS : %d, Average FPS : %.1f", m_FPS, m_TotalAverageFPS);
	}

	m_PrevCount = m_CurrentCount;

	if (!m_IsStop)
	{
		m_fDT = m_fEngineDT;
		m_Time += m_fDT;
	}

	// Global Data 갱신
	g_Data.DeltaTime = m_fDT;
	g_Data.Time = m_Time;
	g_Data.DT_Engine = m_fEngineDT;
	g_Data.Time_Engine = m_fEngineTime;

	// FPS에 따른 색상 변경
	// 60fps 이상: 녹색, 30fps 이상: 노란색, 30fps 미만: 빨간색)
	UINT color = FONT_RGBA(255, 20, 20, 255);
	if (AverageFPS >= 60.f)
	{
		color = FONT_RGBA(20, 255, 20, 255);
	}
	else if (AverageFPS >= 30.f)
	{
		color = FONT_RGBA(255, 255, 20, 255);
	}

	// Tick마다 렌더링은 매번 진행
	CFontMgr::GetInst()->DrawFont(m_TimeInfo, 10, 20, 16, color);
}

void CTimeMgr::SetStopMode(bool PIsStop)
{
	m_IsStop = PIsStop;
	if (m_IsStop)
		m_fDT = 0.f;
}
