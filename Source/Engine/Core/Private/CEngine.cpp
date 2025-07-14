#include "pch.h"
#include "Core/Public/CEngine.h"

#include <psapi.h>
#include <iostream>

#include "Engine/System/Public/Rendering/Device/CDevice.h"
#include "Engine/System/Public/Rendering/Buffer/CInstancingBuffer.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"
#include "Engine/System/Public/Manager/CPathMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CRenderMgr.h"
#include "Engine/System/Public/Manager/CCollisionMgr.h"
#include "Engine/System/Public/Manager/CTaskMgr.h"
#include "Engine/System/Public/Manager/CFontMgr.h"
#include "Engine/System/Public/Manager/CUIMgr.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"
#include "Engine/System/Public/Manager/FLogManager.h"

CEngine::CEngine()
	: m_hMainWnd(nullptr)
	  , m_FMODSystem(nullptr)
{
}

CEngine::~CEngine()
{
	if (nullptr != m_FMODSystem)
	{
		m_FMODSystem->release();

		m_FMODSystem = nullptr;
	}
}

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

	// FMOD 초기화
	System_Create(&m_FMODSystem);
	assert(m_FMODSystem);

	// 32개 채널 생성
	m_FMODSystem->init(32, FMOD_INIT_NORMAL, nullptr);

	// 3D 환경 설정(도플러 효과, 거리 단위, 롤오프 스케일)
	m_FMODSystem->set3DSettings(1.0f, 1.0f, 1.0f);

	// Manager 초기화
	CPathMgr::GetInst()->Init();
	FLogManager::GetInst()->Init();
	CKeyMgr::GetInst()->Init();
	CTimeMgr::GetInst()->Init();
	CAssetMgr::GetInst()->Init();
	CRenderMgr::GetInst()->Init();
	CLevelMgr::GetInst()->Init();
	CFontMgr::GetInst()->Init();
	CInstancingBuffer::GetInst()->Init();

	return S_OK;
}

void CEngine::Progress()
{
	// FMOD Tick
	CSoundMgr::GetInst()->Tick();

	// Engine Tick
	CKeyMgr::GetInst()->Tick();
	CTimeMgr::GetInst()->Tick();
	CLevelMgr::GetInst()->Progress();
	CUIMgr::GetInst()->Tick();
	CCollisionMgr::GetInst()->Tick();

	// Engine Render
	CRenderMgr::GetInst()->Render();
	CFontMgr::GetInst()->Render();

	// Task
	CTaskMgr::GetInst()->Tick();
}

void CEngine::Shutdown()
{
	LOG_INFO("Engine Shutdown Process");
	FLogManager::GetInst()->Shutdown();
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
