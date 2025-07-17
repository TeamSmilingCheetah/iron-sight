#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"

FCollisionManager::FCollisionManager() = default;

FCollisionManager::~FCollisionManager() = default;

/*****************************/
/** Layer Collision Setting **/
/*****************************/

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
void FCollisionManager::ClearCollisionBtwLayerSetting()
{
	memset(MLayerCollisionMatrix, 0, sizeof(UINT) * MAX_LAYER);
}

/****************/
/** Main Logic **/
/****************/

/**
 * @brief 전체 충돌 처리 로직의 처리 함수
 * Create VBH -> Raycast Process -> Broad Phase -> Narrow Phase -> Clean Resource
 */
void FCollisionManager::Tick()
{
	CLevel* CurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	// Early Return (이후로 Current Level 없는 상황은 발생하지 않음)
	if (!CLevelMgr::GetInst()->GetCurrentLevel())
	{
		LOG_INFO("[Collision][Main] No Current Level");
		return;
	}

	// TODO(KHJ): 이 Log 너무 자주 호출됨. 빈번한 로그는 적당히 걸러내는 기능이 필요함
	if (CurrentLevel->GetState() == LEVEL_STATE::STOP)
	{
		LOG_INFO("[Collision][Main] Current Level Not Started");
		return;
	}

	// Create BVH Structure
	LOG_INFO("[Collision][Main] Create Bounding Volume Hirachy Start");
	CreateBVHTree();

	// Raycast Process
	LOG_INFO("[Collision][Main] Raycast Process Start");
	RaycastProcess();

	// Check Collision
	LOG_INFO("[Collision][Main] Broad Phase Start");
	BroadPhase();
	LOG_INFO("[Collision][Main] Narrow Phase Start");
	NarrowPhase();

	// Reset Information
	CleanResource();
	LOG_INFO("[Collision][Main] Collision Process Done");
}

/**
 * @brief 모든 충돌 판정이 완료된 후 사용한 자원을 정리하는 함수
 */
void FCollisionManager::CleanResource()
{
	// Reset Information
	MCandidatePairVector.clear();
}
