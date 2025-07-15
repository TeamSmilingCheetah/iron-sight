#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"

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

/**
 * @brief 충돌 등록 Matrix를 초기화하는 함수
 */
void FCollisionManager::CollisionCheckClear()
{
	memset(MLayerCollisionMatrix, 0, sizeof(UINT) * MAX_LAYER);
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
	CLevel* CurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	// Early Return
	if (!CLevelMgr::GetInst()->GetCurrentLevel())
	{
		LOG_DEBUG("[Collision] Current Level 없음");
		return;
	}

	if (CurrentLevel->GetState() == LEVEL_STATE::STOP)
	{
		LOG_DEBUG("[Collision] Current Level Not Started");
		return;
	}

	// TODO(KHJ): RayCast 처리

	// Build BVH Tree
	vector<CGameObject*> PObjects;
	CurrentLevel->GetAllActiveObjectsInLevel(PObjects);
	LOG_INFO_F("[Collision][Entry] Total Active Objects: {}", PObjects.size());
	BuildBVH(PObjects);

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
	MCandidatePairVector.clear();
}
