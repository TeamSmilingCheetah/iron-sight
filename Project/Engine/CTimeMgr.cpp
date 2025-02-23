#include "pch.h"
#include "CTimeMgr.h"

#include "CEngine.h"
#include "CFontMgr.h"

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
      , m_IsStop(true)
      , m_TimeInfo{}
{
}

CTimeMgr::~CTimeMgr()
{
}

void CTimeMgr::Init()
{
    QueryPerformanceFrequency(&m_Frequency);
    QueryPerformanceCounter(&m_PrevCount);
}

void CTimeMgr::Tick()
{
    QueryPerformanceCounter(&m_CurrentCount);

    m_fEngineDT = static_cast<float>(m_CurrentCount.QuadPart - m_PrevCount.QuadPart) / static_cast<
        float>(m_Frequency.QuadPart);

    // DT ����, 60 ���� 1 ���� ũ�� ���ϰ� ����
    if ((1.f / 60.f) < m_fEngineDT)
        m_fEngineDT = (1.f / 60.f);

    // �ð� ����
    m_fEngineTime += m_fEngineDT;

    // �ð� ����
    m_Second += m_fEngineDT;

    // �Լ� ȣ��Ƚ��
    ++m_FPS;

    if (1. < m_Second)
    {
        m_Second -= 1.;

        // ������ Ÿ��Ʋ�� FPS �� DeltaTime ǥ��
        swprintf_s(m_TimeInfo, L"DeltaTime : %f, FPS : %d ", m_fEngineDT, m_FPS);
        m_FPS = 0;
    }

    m_PrevCount = m_CurrentCount;

    if (!m_IsStop)
    {
        m_fDT = m_fEngineDT;
        m_Time += m_fDT;
    }


    // GlobalData ����
    g_Data.DeltaTime = m_fDT;
    g_Data.Time = m_Time;

    g_Data.DT_Engine = m_fEngineDT;
    g_Data.Time_Engine = m_fEngineTime;
}

void CTimeMgr::Render()
{
    CFontMgr::GetInst()->DrawFont(m_TimeInfo, 10, 20, 16, FONT_RGBA(255, 20, 20, 255));
}
