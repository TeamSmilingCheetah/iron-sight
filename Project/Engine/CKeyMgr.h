#pragma once

enum class KEY
{
    W,
    S,
    A,
    D,
    Z,
    Y,

    LEFT,
    RIGHT,
    UP,
    DOWN,

    SPACE,
    ENTER,

    LSHIFT,

    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,
    NUM_0,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,


    LBTN,
    RBTN,

    END,
};


enum class KEY_STATE
{
    TAP, // �� ���� ����
    PRESSED, // ��� �����ִ� ����
    RELEASED, // �� �� ����
    NONE, // �������� ���� ����
};


struct tKeyInfo
{
    KEY_STATE State;
    bool PrevPressed;
};


class CKeyMgr
    : public singleton<CKeyMgr>
{
    SINGLE(CKeyMgr);
    vector<tKeyInfo> m_vecKey;

    Vec2 m_MousePos; // ���� ���콺 ��ġ
    Vec2 m_MousePrevPos; // ���� ������ ���콺 ��ġ
    Vec2 m_MouseDir; // ���콺 ��ǥ�� �̵��� ����

public:
    void Init();
    void Tick();

    KEY_STATE GetKeyState(KEY _Key) { return m_vecKey[static_cast<int>(_Key)].State; }
    Vec2 GetMousePos() { return m_MousePos; }
    Vec2 GetMouseDir() { return m_MouseDir; }
};
