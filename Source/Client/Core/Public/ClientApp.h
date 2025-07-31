#pragma once

class FAppWindow;

/**
 * @brief Main Client Class
 * Application Entry Point, Manage Overall Execution Flow
 *
 * @var AcceleratorTable 키보드 단축키 테이블 핸들
 * @var MainMessage 윈도우 메시지 구조체
 * @var Window 윈도우 객체 포인터
 */
class FClientApp
{
private:
	HACCEL AcceleratorTable;
	MSG MainMessage;
	FAppWindow* Window;

private:
	int InitializeSystem() const;
	static void UpdateSystem();
	void MainLoop();

public:
	int Run(HINSTANCE InInstanceHandle, int InCmdShow);

	// Special Member Function
	FClientApp();
	~FClientApp();
};
