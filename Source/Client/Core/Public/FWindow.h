#pragma once
#include <windows.h>

class FClientApp;

/**
 * @brief 윈도우 생성 및 메시지 처리를 담당하는 클래스
 */
class FWindow
{
private:
	FClientApp* MOwner;
	HINSTANCE MInstanceHandle;
	HWND MMainWindowHandle;

private:
	// Windows Callback Function
	static LRESULT CALLBACK WndProc(HWND PWindowHandle, UINT PMessage, WPARAM PWParam, LPARAM PLParam);
	static INT_PTR CALLBACK About(HWND PDialogHandle, UINT PMessage, WPARAM PWParam, LPARAM PLParam);

	// Message Handle Function
	static FWindow* GetWindowInstance(HWND PWindowHandle, UINT PMessage, LPARAM PLParam);
	static LRESULT HandleNCCreate(HWND PWindowHandle, LPARAM PLParam);
	static LRESULT HandleCommand(HWND PWindowHandle, WPARAM PWParam, const FWindow* PWindowInstance);
	static LRESULT HandlePaint(HWND PWindowHandle);

public:
	bool Init(HINSTANCE PInstance, int PCmdShow);
	void InitializeConsole();

	// Getter & Setter
	HWND GetWindowHandle() const { return MMainWindowHandle; }
	HINSTANCE GetInstanceHandle() const { return MInstanceHandle; }

	// Special Member Function
	FWindow(FClientApp* POwner);
	~FWindow();
};
