#pragma once

class CEngine
	: public singleton<CEngine>
{
	SINGLE(CEngine)
private:
	HWND	m_hMainWnd;
	Vec2	m_Resolution;

	// FMOD 관리자 클래스
	FMOD::System* m_FMODSystem;  

public:
	HWND GetMainWnd() { return m_hMainWnd; }
	FMOD::System* GetFMODSystem() { return m_FMODSystem; }

public:
	int Init(HWND _hWnd, UINT _Width, UINT _Height
		   , GAMEOBJECT_SAVE _SaveFunc, GAMEOBJECT_LOAD _LoadFunc);
	void Progress();
};
