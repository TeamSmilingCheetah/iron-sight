#pragma once


class CTimeMgr
    : public singleton<CTimeMgr>
{
    SINGLE(CTimeMgr);
    LARGE_INTEGER m_Frequency; // 1�ʴ� ī���� ���� ��
    LARGE_INTEGER m_CurrentCount; // ���� ī����
    LARGE_INTEGER m_PrevCount; // ���� ������ ī����

    float m_fDT; // DeltaTime : 1�����ӿ� �ɸ� �ð�
    float m_Time; // ���� �ð�

    float m_fEngineDT;
    float m_fEngineTime;

    double m_Second; // 1�� üũ �뵵 �����ð�

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
