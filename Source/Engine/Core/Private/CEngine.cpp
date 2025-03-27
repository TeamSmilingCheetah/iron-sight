#include "pch.h"
#include "Core/Public/CEngine.h"
#include "System/Public/Rendering/Device/CDevice.h"
#include "System/Public/Manager/CPathMgr.h"
#include "System/Public/Manager/CTimeMgr.h"
#include "System/Public/Manager/CKeyMgr.h"
#include "System/Public/Manager/CAssetMgr.h"
#include "System/Public/Manager/CLevelMgr.h"
#include "System/Public/Manager/CRenderMgr.h"
#include "System/Public/Manager/CCollisionMgr.h"
#include "System/Public/Manager/CTaskMgr.h"
#include "System/Public/Manager/CFontMgr.h"
#include "System/Public/Manager/CUIMgr.h"
#include "System/Public/Asset/Prefab/CPrefab.h"
#include "System/Public/Rendering/Buffer/CInstancingBuffer.h"

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
	m_FMODSystem->init(32, FMOD_DEFAULT, nullptr);

	// Manager 초기화
	CPathMgr::GetInst()->Init();
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
	m_FMODSystem->update();

	// Engine Tick
	CKeyMgr::GetInst()->Tick();
	CTimeMgr::GetInst()->Tick();
	CLevelMgr::GetInst()->Progress();
	CCollisionMgr::GetInst()->Tick();
	CUIMgr::GetInst()->Tick();

	// Engine Render
	CRenderMgr::GetInst()->Render();
	CFontMgr::GetInst()->Render();

	// Task
	CTaskMgr::GetInst()->Tick();
}
