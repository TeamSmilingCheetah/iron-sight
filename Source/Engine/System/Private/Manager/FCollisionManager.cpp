#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "System/Public/Manager/CLevelMgr.h"

FCollisionManager::FCollisionManager() = default;

FCollisionManager::~FCollisionManager() = default;

/*******************/
/** Level Setting **/
/*******************/

/**
 * @brief 레이어 간의 충돌 설정을 추가하는 함수
 */
void FCollisionManager::ActiveLayerCollision(UINT PLeft, UINT PRight)
{
	UINT Row = PLeft;
	UINT Col = PRight;

	if (Col < Row)
	{
		Row = PRight;
		Col = PLeft;
	}

	MLayerCollisionMatrix[Row] |= (1 << Col);
}

/****************/
/** Main Logic **/
/****************/

/**
 * @brief 전체 충돌 처리 로직의 처리 함수
 * Broad Check -> Narrow Check -> Clean Resource
 */
void FCollisionManager::Tick()
{
	// Early Return
	if (!CLevelMgr::GetInst()->GetCurrentLevel())
	{
		LOG_DEBUG("[Collision] Current Level 없음");
		return;
	}

	// Check Collision
	LOG_DEBUG("[Collision] Broad Check Start");
	CheckBroadPhase();
	LOG_DEBUG("[Collision] Narrow Check Start");
	CheckNarrowPhase();

	// Reset Information
	CleanResource();
	LOG_DEBUG("[Collision] Collision Check Done");
}

/**
 * @brief 모든 충돌 판정이 완료된 후 사용한 자원을 정리하는 함수
 */
void FCollisionManager::CleanResource()
{
	// Reset Information
	MLandscapeObject.clear();
	RayCollsionInfo.clear();
	MCandidatePairVector.clear();
}
