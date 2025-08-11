#include "pch.h"
#include "Core/Public/CEngine.h"

#include <psapi.h>

#include "Engine/System/Public/Asset/Prefab/CPrefab.h"
#include "Engine/System/Public/Manager/CPathMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CRenderMgr.h"
#include "Engine/System/Public/Manager/CollisionManager.h"
#include "Engine/System/Public/Manager/CTaskMgr.h"
#include "Engine/System/Public/Manager/CFontMgr.h"
#include "Engine/System/Public/Manager/CUIMgr.h"
#include "Engine/System/Public/Manager/SoundManager.h"
#include "Engine/System/Public/Manager/LogManager.h"
#include "Engine/System/Public/Rendering/Device/CDevice.h"
#include "Engine/System/Public/Rendering/Buffer/CInstancingBuffer.h"

CEngine::CEngine() = default;

CEngine::~CEngine() = default;

int CEngine::Init(HWND _hWnd, UINT _Width, UINT _Height
                  , GAMEOBJECT_SAVE _SaveFunc, GAMEOBJECT_LOAD _LoadFunc)
{
	m_hMainWnd = _hWnd;

	m_Resolution.x = static_cast<float>(_Width);
	m_Resolution.y = static_cast<float>(_Height);

	// GameObject Save Load 함수 포인터
	CPrefab::g_ObjectSave = _SaveFunc;
	CPrefab::g_ObjectLoad = _LoadFunc;

	RECT rt = {0, 0, static_cast<int>(m_Resolution.x), static_cast<int>(m_Resolution.y)};
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, !!GetMenu(m_hMainWnd));
	SetWindowPos(m_hMainWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);

	if (FAILED(CDevice::GetInst()->Init(m_hMainWnd, m_Resolution)))
	{
		return E_FAIL;
	}

	// Manager 초기화
	CPathMgr::GetInst()->Init();
	LogManager::GetInst()->Init();
	CKeyMgr::GetInst()->Init();
	CTimeMgr::GetInst()->Init();
	CAssetMgr::GetInst()->Init();
	FSoundManager::GetInst()->Init();
	CRenderMgr::GetInst()->Init();
	CLevelMgr::GetInst()->Init();
	CFontMgr::GetInst()->Init();
	CInstancingBuffer::GetInst()->Init();

	return S_OK;
}

void CEngine::Progress()
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

void CEngine::Shutdown()
{
	LOG_INFO("Engine Shutdown Process");
	LogManager::GetInst()->Shutdown();
}

void CEngine::PrintMemoryUsage(const string& PText)
{
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
	{
		LOG_INFO(PText);
		LOG_INFO_F("WorkingSet: {:.2f}MB", pmc.WorkingSetSize / (1024.0 * 1024.0));
		LOG_INFO_F("PrivateUsage: {:.2f}MB", pmc.PrivateUsage / (1024.0 * 1024.0));
	}
	else
	{
		LOG_ERROR("Failed To Get Memory Info");
	}
}
