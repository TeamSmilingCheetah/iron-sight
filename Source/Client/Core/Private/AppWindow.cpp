#include "pch.h"
#include "Client/Core/Public/AppWindow.h"

#include "Client/Build/Resource.h"
#include "Client/imgui/imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

FAppWindow::FAppWindow(FClientApp* InOwner)
	: Owner(InOwner), InstanceHandle(nullptr), MainWindowHandle(nullptr)
{
}

FAppWindow::~FAppWindow() = default;

bool FAppWindow::Init(HINSTANCE InInstance, int InCmdShow)
{
	InstanceHandle = InInstance;

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = InInstance;
	wcex.hIcon = LoadIcon(InInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"ProjectIronSight";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	MainWindowHandle = CreateWindowW(L"ProjectIronSight", L"", WS_OVERLAPPEDWINDOW,
	                                  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, InInstance, this);

	if (!MainWindowHandle)
	{
		return false;
	}

	ShowWindow(MainWindowHandle, InCmdShow);
	UpdateWindow(MainWindowHandle);
	SetWindowTextW(MainWindowHandle, L"Project Iron Sight");

	return true;
}

/**
 * @brief Initialize Console & Redirect IO
 */
void FAppWindow::InitializeConsole()
{
	// Error Handle
	if (!AllocConsole())
	{
		MessageBoxW(nullptr, L"콘솔 생성 실패", L"Error", 0);
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

FAppWindow* FAppWindow::GetWindowInstance(HWND InWindowHandle, UINT InMessage, LPARAM InLParam)
{
	if (InMessage == WM_NCCREATE)
	{
		CREATESTRUCT* CreateStruct = reinterpret_cast<CREATESTRUCT*>(InLParam);
		FAppWindow* WindowInstance = static_cast<FAppWindow*>(CreateStruct->lpCreateParams);
		SetWindowLongPtr(InWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(WindowInstance));

		return WindowInstance;
	}

	return reinterpret_cast<FAppWindow*>(GetWindowLongPtr(InWindowHandle, GWLP_USERDATA));
}

LRESULT FAppWindow::HandleNCCreate(HWND InWindowHandle, LPARAM InLParam)
{
	CREATESTRUCT* CreateStruct = reinterpret_cast<CREATESTRUCT*>(InLParam);
	FAppWindow* WindowInstance = static_cast<FAppWindow*>(CreateStruct->lpCreateParams);
	SetWindowLongPtr(InWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(WindowInstance));

	return TRUE;
}

LRESULT FAppWindow::HandleCommand(HWND InWindowHandle, WPARAM InWParam, const FAppWindow* InWindowInstance)
{
	int WindowMessageIdentifier = LOWORD(InWParam);

	switch (WindowMessageIdentifier)
	{
	case IDM_ABOUT:
		DialogBox(InWindowInstance->InstanceHandle, MAKEINTRESOURCE(IDD_ABOUTBOX), InWindowHandle, About);
		return 0;

	case IDM_EXIT:
		DestroyWindow(InWindowHandle);
		return 0;

	default:
		return DefWindowProc(InWindowHandle, WM_COMMAND, InWParam, 0);
	}
}

LRESULT FAppWindow::HandlePaint(HWND InWindowHandle)
{
	PAINTSTRUCT PaintStruct;
	BeginPaint(InWindowHandle, &PaintStruct);
	EndPaint(InWindowHandle, &PaintStruct);

	return 0;
}

LRESULT CALLBACK FAppWindow::WndProc(HWND InWindowHandle, UINT InMessage, WPARAM InWParam, LPARAM InLParam)
{
	// Handle ImGui Message
	if (ImGui_ImplWin32_WndProcHandler(InWindowHandle, InMessage, InWParam, InLParam))
	{
		return true;
	}

	// Get Window Instance
	FAppWindow* WindowInstance = GetWindowInstance(InWindowHandle, InMessage, InLParam);
	if (!WindowInstance)
	{
		return DefWindowProc(InWindowHandle, InMessage, InWParam, InLParam);
	}

	// Message Control
	switch (InMessage)
	{
	case WM_NCCREATE:
		return HandleNCCreate(InWindowHandle, InLParam);

	case WM_COMMAND:
		return HandleCommand(InWindowHandle, InWParam, WindowInstance);

	case WM_PAINT:
		return HandlePaint(InWindowHandle);

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(InWindowHandle, InMessage, InWParam, InLParam);
	}
}

INT_PTR CALLBACK FAppWindow::About(HWND InDialogHandle, UINT InMessage, WPARAM InWParam, LPARAM InLParam)
{
	UNREFERENCED_PARAMETER(InLParam);

	switch (InMessage)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(InWParam) == IDOK || LOWORD(InWParam) == IDCANCEL)
		{
			EndDialog(InDialogHandle, LOWORD(InWParam));
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

void FAppWindow::SetTitle(const wstring& InNewTitle) const
{
	SetWindowTextW(MainWindowHandle, InNewTitle.c_str());
}
