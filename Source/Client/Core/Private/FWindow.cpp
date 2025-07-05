#include "pch.h"
#include "Client/Core/Public/FWindow.h"

#include "Client/Build/Resource.h"
#include "Client/imgui/imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

FWindow::FWindow(FClientApp* POwner)
	: MOwner(POwner), MInstanceHandle(nullptr), MMainWindowHandle(nullptr)
{
}

FWindow::~FWindow() = default;

bool FWindow::Init(HINSTANCE PInstance, int PCmdShow)
{
	MInstanceHandle = PInstance;

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = PInstance;
	wcex.hIcon = LoadIcon(PInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"Key";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	MMainWindowHandle = CreateWindowW(L"Key", L"Game", WS_OVERLAPPEDWINDOW,
	                                  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, PInstance, this);

	if (!MMainWindowHandle)
	{
		return false;
	}

	ShowWindow(MMainWindowHandle, PCmdShow);
	UpdateWindow(MMainWindowHandle);

	return true;
}

/**
 * @brief Initialize Console & Redirect IO
 */
void FWindow::InitializeConsole()
{
	// Error Handle
	if (!AllocConsole())
	{
		MessageBoxA(nullptr, "콘솔 생성 실패", "Error", 0);
	}

	// Console 출력 지정
	FILE* FilePtr;
	(void)freopen_s(&FilePtr, "CONOUT$", "w", stdout);
	(void)freopen_s(&FilePtr, "CONOUT$", "w", stderr);
	(void)freopen_s(&FilePtr, "CONIN$", "r", stdin);

	// Console Menu Setting
	HWND ConsoleWindow = GetConsoleWindow();
	HMENU MenuHandle = GetSystemMenu(ConsoleWindow, FALSE);
	if (MenuHandle != nullptr)
	{
		EnableMenuItem(MenuHandle, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
		DeleteMenu(MenuHandle, SC_CLOSE, MF_BYCOMMAND);
	}
}

FWindow* FWindow::GetWindowInstance(HWND PWindowHandle, UINT PMessage, LPARAM PLParam)
{
	if (PMessage == WM_NCCREATE)
	{
		CREATESTRUCT* CreateStruct = reinterpret_cast<CREATESTRUCT*>(PLParam);
		FWindow* WindowInstance = static_cast<FWindow*>(CreateStruct->lpCreateParams);
		SetWindowLongPtr(PWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(WindowInstance));

		return WindowInstance;
	}

	return reinterpret_cast<FWindow*>(GetWindowLongPtr(PWindowHandle, GWLP_USERDATA));
}

LRESULT FWindow::HandleNCCreate(HWND PWindowHandle, LPARAM PLParam)
{
	CREATESTRUCT* CreateStruct = reinterpret_cast<CREATESTRUCT*>(PLParam);
	FWindow* WindowInstance = static_cast<FWindow*>(CreateStruct->lpCreateParams);
	SetWindowLongPtr(PWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(WindowInstance));

	return TRUE;
}

LRESULT FWindow::HandleCommand(HWND PWindowHandle, WPARAM PWParam, const FWindow* PWindowInstance)
{
	int WindowMessageIdentifier = LOWORD(PWParam);

	switch (WindowMessageIdentifier)
	{
	case IDM_ABOUT:
		DialogBox(PWindowInstance->MInstanceHandle, MAKEINTRESOURCE(IDD_ABOUTBOX), PWindowHandle, About);
		return 0;

	case IDM_EXIT:
		DestroyWindow(PWindowHandle);
		return 0;

	default:
		return DefWindowProc(PWindowHandle, WM_COMMAND, PWParam, 0);
	}
}

LRESULT FWindow::HandlePaint(HWND PWindowHandle)
{
	PAINTSTRUCT PaintStruct;
	BeginPaint(PWindowHandle, &PaintStruct);
	EndPaint(PWindowHandle, &PaintStruct);

	return 0;
}

LRESULT CALLBACK FWindow::WndProc(HWND PWindowHandle, UINT PMessage, WPARAM PWParam, LPARAM PLParam)
{
	// Handle ImGui Message
	if (ImGui_ImplWin32_WndProcHandler(PWindowHandle, PMessage, PWParam, PLParam))
	{
		return true;
	}

	// Get Window Instance
	FWindow* WindowInstance = GetWindowInstance(PWindowHandle, PMessage, PLParam);
	if (!WindowInstance)
	{
		return DefWindowProc(PWindowHandle, PMessage, PWParam, PLParam);
	}

	// Message Control
	switch (PMessage)
	{
	case WM_NCCREATE:
		return HandleNCCreate(PWindowHandle, PLParam);

	case WM_COMMAND:
		return HandleCommand(PWindowHandle, PWParam, WindowInstance);

	case WM_PAINT:
		return HandlePaint(PWindowHandle);

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(PWindowHandle, PMessage, PWParam, PLParam);
	}
}

INT_PTR CALLBACK FWindow::About(HWND PDialogHandle, UINT PMessage, WPARAM PWParam, LPARAM PLParam)
{
	UNREFERENCED_PARAMETER(PLParam);

	switch (PMessage)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(PWParam) == IDOK || LOWORD(PWParam) == IDCANCEL)
		{
			EndDialog(PDialogHandle, LOWORD(PWParam));
			return TRUE;
		}
		break;

	default:
		{
			// No Default
		}
	}

	return FALSE;
}
