#include "pch.h"

#include "System/Public/Manager/CKeyMgr.h"
#include "System/Public/Manager/RenderManager.h"

/**
 * @brief 현재 활성화된 메인 카메라를 반환하는 함수
 */
CCamera* FRenderManager::GetMainCamera() const
{
	// Editor Mode
	if (bIsEditorMode)
	{
		return EditorCamera;
	}
	// Use Main Camera
	if (!CameraVector.empty())
	{
		return CameraVector[0];
	}

	LOG_ERROR("[Render][Camera] Can't Find Camera In Non Editor Mode");
	return nullptr;
}

void FRenderManager::Render()
{
	bIsDebugRender = false;

	// SetUp By Build
#ifdef _DEBUG
	// Trigger Debug Render If Debug Build
	if (KEY_TAP(KEY::F9))
	{
		bIsDebugRender = !bIsDebugRender;
	}
#else
	if (bIsEditorMode)
	{
		bIsEditorMode = false;
	}
#endif

	RenderStart();

	if (bIsEditorMode)
	{
		RenderEditor();
	}
	else
	{
		RenderPlay();
	}

	RenderDebug();

	RenderClear();
}

void FRenderManager::RenderStart()
{
}

void FRenderManager::RenderEditor()
{
}

void FRenderManager::RenderPlay()
{
}

void FRenderManager::RenderDebug()
{
}

void FRenderManager::RenderClear()
{
}
