#include "pch.h"
#include "Client/Core/Public/FClientApp.h"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "Client/Build/Resource.h"
#include "Client/imgui/imgui.h"
#include "Client/System/Public/CImGuiMgr.h"
#include "Client/Core/Public/CEditorMgr.h"
#include "Engine/Core/Public/CEngine.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Game/Level/Public/TestLevel.h"

FClientApp::FClientApp() = default;

FClientApp::~FClientApp() = default;

/**
 * @brief Client Main Runtime Function
 * App 초기화, Main Loop 실행을 통한 전체 Cycle
 *
 * @param PInstanceHandle [IN] Process Instance Handle
 * @param PCmdShow [IN] Window Display Method
 *
 * @return Program Termination Code
 */
int FClientApp::Run(HINSTANCE PInstanceHandle, int PCmdShow)
{
	// TODO(KHJ): 어느 정도 정리되면 메모리 체크는 Debug 모드에서만
// #ifdef _DEBUG
	// Memory Leak Detection & Report
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
// #endif

	// Window Object Initialize
	MWindow = new FWindow(this);
	if (!MWindow->Init(PInstanceHandle, PCmdShow))
	{
		assert("Window Creation Failed");
		return 0;
	}

	// Create Console
	MWindow->InitializeConsole();

	// Keyboard Accelerator Table Setting
	MAcceleratorTable = LoadAccelerators(PInstanceHandle, MAKEINTRESOURCE(IDC_CLIENT));

	// Initialize Base System
	int InitResult = InitializeSystem();
	if (InitResult != S_OK)
	{
		assert("Initialize Failed");
		return 0;
	}

	// Execute Main Loop
	MainLoop();

	// Termination Process
	SetCurrentDirectory(CPathMgr::GetInst()->GetBinPath().c_str());
	CEngine::GetInst()->Shutdown();
	delete MWindow;

	return static_cast<int>(MMainMessage.wParam);
}

/**
 * @brief Initialize System For Game Execution
 */
int FClientApp::InitializeSystem() const
{
	// Initialize Engine
	if (FAILED(
		CEngine::GetInst()->Init(MWindow->GetWindowHandle(), 1280, 768,
			&CLevelMgr::SaveGameObject, &CLevelMgr::LoadGameObject)))
	{
		assert("Engine Initialize Failed");
		return E_FAIL;
	}

#ifdef _DEBUG
	// Debug Mode
	// Initialize ImGui & Editor
	CImGuiMgr::GetInst()->Init();
	CEditorMgr::GetInst()->Init();

	// Load Test Level As Default
	TestLevel::CreateTestLevel();

#else
	// Release Mode
	// Try To Load Starting Level
	CLevel* StartingLevel = CLevelMgr::LoadLevel(L"Level/StartingLevel.lv");

	// Play Loaded Level If Success
	if (StartingLevel) {
		ChangeLevel(StartingLevel, LEVEL_STATE::PLAY);

	// Load Test Level If Failed
	} else {
		TestLevel::CreateTestLevel();
		ChangeLevelState(LEVEL_STATE::PLAY);
	}
#endif

	return S_OK;
}

/**
 * @brief Update System While Game Processing
 */
void FClientApp::UpdateSystem() const
{
	// System Progress
	CEngine::GetInst()->Progress();

#ifdef _DEBUG
	// Editor & Imgui On Debug Mode
	CEditorMgr::GetInst()->Progress();
	CImGuiMgr::GetInst()->Progress();
#endif

	// Screen Output Processing
	CDevice::GetInst()->Present();
}

/**
 * @brief Execute Main Message Loop
 * 윈도우 메시지 처리 및 게임 시스템 업데이트를 담당
 */
void FClientApp::MainLoop()
{
	while (true)
	{
		// Async Message Process
		if (PeekMessage(&MMainMessage, nullptr, 0, 0, PM_REMOVE))
		{
			// Process Termination
			if (MMainMessage.message == WM_QUIT)
			{
				break;
			}
			// Shortcut Key Processing
			if (!TranslateAccelerator(MMainMessage.hwnd, MAcceleratorTable, &MMainMessage))
			{
				TranslateMessage(&MMainMessage);
				DispatchMessage(&MMainMessage);
			}
		}
		// Game System Update
		else
		{
			UpdateSystem();
		}
	}
}
