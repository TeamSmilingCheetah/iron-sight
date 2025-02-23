#include "pch.h"
#include "CKeyMgr.h"

#include "CEngine.h"

int g_arrKeyValue[static_cast<int>(KEY::END)]
    =
    {
        'W',
        'S',
        'A',
        'D',
        'Z',
        'Y',

        VK_LEFT,
        VK_RIGHT,
        VK_UP,
        VK_DOWN,

        VK_SPACE,
        VK_RETURN,

        VK_LSHIFT,

        VK_NUMPAD1,
        VK_NUMPAD2,
        VK_NUMPAD3,
        VK_NUMPAD4,
        VK_NUMPAD5,
        VK_NUMPAD6,
        VK_NUMPAD7,
        VK_NUMPAD8,
        VK_NUMPAD9,
        VK_NUMPAD0,

        VK_F1,
        VK_F2,
        VK_F3,
        VK_F4,
        VK_F5,
        VK_F6,
        VK_F7,
        VK_F8,
        VK_F9,
        VK_F10,
        VK_F11,
        VK_F12,


        VK_LBUTTON,
        VK_RBUTTON,
    };


CKeyMgr::CKeyMgr()
{
}

CKeyMgr::~CKeyMgr()
{
}


void CKeyMgr::Init()
{
    for (int i = 0; i < static_cast<int>(KEY::END); ++i)
    {
        m_vecKey.push_back(tKeyInfo{KEY_STATE::NONE, false});
    }
}

void CKeyMgr::Tick()
{
    // ��Ŀ������ �����찡 �ִ� ��쿡��
    if (GetFocus())
    {
        for (size_t i = 0; i < m_vecKey.size(); ++i)
        {
            // Ű�� �����ִ��� Ȯ��
            if (GetAsyncKeyState(g_arrKeyValue[i]) & 0x8001)
            {
                // Ű�� �����ְ�, �������� �������� �ʾҴ�.
                if (false == m_vecKey[i].PrevPressed)
                {
                    m_vecKey[i].State = KEY_STATE::TAP;
                }

                // Ű�� �����ְ�, �������� �����־���.
                else
                {
                    m_vecKey[i].State = KEY_STATE::PRESSED;
                }

                m_vecKey[i].PrevPressed = true;
            }

            // Ű�� �������� �ʴٸ�
            else
            {
                // �������� �����־���.
                if (m_vecKey[i].PrevPressed)
                {
                    m_vecKey[i].State = KEY_STATE::RELEASED;
                }
                else
                {
                    m_vecKey[i].State = KEY_STATE::NONE;
                }

                m_vecKey[i].PrevPressed = false;
            }
        }

        m_MousePrevPos = m_MousePos;

        POINT Pos;
        GetCursorPos(&Pos);
        ScreenToClient(CEngine::GetInst()->GetMainWnd(), &Pos);
        m_MousePos = Vec2(static_cast<float>(Pos.x), static_cast<float>(Pos.y));
        m_MouseDir = m_MousePos - m_MousePrevPos;
    }

    // ��Ŀ�� ���� �����찡 ������
    else
    {
        for (size_t i = 0; i < m_vecKey.size(); ++i)
        {
            if (KEY_STATE::TAP == m_vecKey[i].State || KEY_STATE::PRESSED == m_vecKey[i].State)
            {
                m_vecKey[i].State = KEY_STATE::RELEASED;
            }

            else if (KEY_STATE::RELEASED == m_vecKey[i].State)
            {
                m_vecKey[i].State = KEY_STATE::NONE;
            }

            m_vecKey[i].PrevPressed = false;
        }
    }
}
