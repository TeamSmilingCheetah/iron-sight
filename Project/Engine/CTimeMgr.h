#pragma once


class CTimeMgr
    : public singleton<CTimeMgr>
{
    SINGLE(CTimeMgr);
    LARGE_INTEGER m_Frequency; // 1초당 카운팅 가능 수
    LARGE_INTEGER m_CurrentCount; // 현재 카운팅
    LARGE_INTEGER m_PrevCount; // 이전 프레임 카운팅

    float m_fDT; // DeltaTime : 1프레임에 걸린 시간
    float m_Time; // 누적 시간

    float m_fEngineDT;
    float m_fEngineTime;

    double m_Second; // 1초 체크 용도 누적 시간

    UINT m_FPS;

    bool m_IsStop;

    wchar_t m_TimeInfo[255];

public:
    void Init();
    void Tick();
    void Render();

    void SetStopMode(bool _IsStop)
    {
        m_IsStop = _IsStop;
        if (m_IsStop)
            m_fDT = 0.f;
    }

    float GetDeltaTime() { return m_fDT; }
    float GetEngineDeltaTime() { return m_fEngineDT; }
};
