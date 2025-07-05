#pragma once
#include <windows.h>
#include "FWindow.h"

/**
 * @brief Main Client Class
 * Application Entry Point, Manage Overall Execution Flow
 *
 * @var MAcceleratorTable 키보드 단축키 테이블 핸들
 * @var MMainMessage 윈도우 메시지 구조체
 * @var MWindow 윈도우 객체 포인터
 */
class FClientApp
{
private:
	HACCEL MAcceleratorTable;
	MSG MMainMessage;
	FWindow* MWindow;

private:
	int InitializeSystem() const;
	void UpdateSystem() const;
	void MainLoop();

public:
	int Run(HINSTANCE PInstanceHandle, int PCmdShow);

	// Special Member Function
	FClientApp();
	~FClientApp();
};
