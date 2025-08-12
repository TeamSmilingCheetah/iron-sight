#include "pch.h"
#include "Core/Public/Engine.h"

#include <psapi.h>

#include "Engine/System/Public/Manager/CFontMgr.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CollisionManager.h"
#include "Engine/System/Public/Manager/CRenderMgr.h"
#include "Engine/System/Public/Manager/CTaskMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CUIMgr.h"
#include "Engine/System/Public/Manager/SoundManager.h"
#include "Engine/System/Public/Rendering/Buffer/CInstancingBuffer.h"

FEngine::FEngine() = default;

FEngine::~FEngine() = default;

int FEngine::Init(HWND InWindowHandle, UINT InWidth, UINT InHeight
                  , GAMEOBJECT_SAVE InSaveFunction, GAMEOBJECT_LOAD InLoadFunction)
{
	MainWindowHandle = InWindowHandle;

	Resolution.x = static_cast<float>(InWidth);
	Resolution.y = static_cast<float>(InHeight);

	// GameObject Save Load 함수 포인터
	CPrefab::g_ObjectSave = InSaveFunction;
	CPrefab::g_ObjectLoad = InLoadFunction;

	RECT rt = {0, 0, static_cast<int>(Resolution.x), static_cast<int>(Resolution.y)};
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, !!GetMenu(MainWindowHandle));
	SetWindowPos(MainWindowHandle, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);

	if (FAILED(CDevice::GetInst()->Init(MainWindowHandle, Resolution)))
	{
		return E_FAIL;
	}

	// Manager 초기화
	CPathMgr::GetInst()->Init();
	LogManager::GetInst()->Init();
	CKeyMgr::GetInst()->Init();
	CTimeMgr::GetInst()->Init();
	FAssetManager::GetInst()->Init();
	FSoundManager::GetInst()->Init();
	CRenderMgr::GetInst()->Init();
	CLevelMgr::GetInst()->Init();
	CFontMgr::GetInst()->Init();
	CInstancingBuffer::GetInst()->Init();

	return S_OK;
}

void FEngine::Progress()
{
	// FMOD Tick
	FSoundManager::GetInst()->Tick();

	// Engine Tick
	CKeyMgr::GetInst()->Tick();
	CTimeMgr::GetInst()->Tick();
	CLevelMgr::GetInst()->Progress();
	CUIMgr::GetInst()->Tick();
	FCollisionManager::GetInst()->Tick();

	// Engine Render
	CRenderMgr::GetInst()->Render();
	CFontMgr::GetInst()->Render();

	// Task
	CTaskMgr::GetInst()->Tick();
}

void FEngine::Shutdown()
{
	LOG_INFO("[Engine][Main] Engine Shutdown Start");
	FSoundManager::GetInst()->Shutdown();
	FAssetManager::GetInst()->Shutdown();

	LOG_INFO("[Engine][Main] Engine Shutdown Finish");
	LogManager::GetInst()->Shutdown();
}

void FEngine::PrintMemoryUsage(const string& InText)
{
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
	{
		LOG_INFO(InText);
		LOG_INFO_F("WorkingSet: {:.2f}MB", pmc.WorkingSetSize / (1024.0 * 1024.0));
		LOG_INFO_F("PrivateUsage: {:.2f}MB", pmc.PrivateUsage / (1024.0 * 1024.0));
	}
	else
	{
		LOG_ERROR("Failed To Get Memory Info");
	}
}
