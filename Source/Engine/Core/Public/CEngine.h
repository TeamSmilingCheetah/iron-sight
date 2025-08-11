#pragma once

class CEngine
	: public singleton<CEngine>
{
	SINGLE(CEngine)
	HWND m_hMainWnd;
	Vec2 m_Resolution;

public:
	HWND GetMainWnd() { return m_hMainWnd; }

	int Init(HWND _hWnd, UINT _Width, UINT _Height
			 , GAMEOBJECT_SAVE _SaveFunc, GAMEOBJECT_LOAD _LoadFunc);
	void Progress();
	void Shutdown();

	// TEST: memory profiling
	static void PrintMemoryUsage(const string& PText);
};
