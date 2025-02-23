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
	TAP,		// 막 눌린 상태
	PRESSED,	// 계속 눌려있는 상태
	RELEASED,	// 막 뗀 상태
	NONE,		// 눌려있지 않은 상태
};


struct tKeyInfo
{
	KEY_STATE	State;
	bool		PrevPressed;
};


class CKeyMgr
	: public singleton<CKeyMgr>
{
	SINGLE(CKeyMgr);
private:
	vector<tKeyInfo>	m_vecKey;

	Vec2				m_MousePos;		// 현재 마우스 위치
	Vec2				m_MousePrevPos;	// 이전 프레임 마우스 위치
	Vec2				m_MouseDir;		// 마우스 좌표가 이동한 방향

public:
	void Init();
	void Tick();

	KEY_STATE GetKeyState(KEY _Key) { return m_vecKey[(int)_Key].State; }
	Vec2 GetMousePos() { return m_MousePos; }
	Vec2 GetMouseDir() { return m_MouseDir; }
};

