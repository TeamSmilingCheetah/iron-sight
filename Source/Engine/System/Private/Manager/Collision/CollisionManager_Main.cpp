#include "pch.h"
#include "Engine/System/Public/Manager/CollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"

FCollisionManager::FCollisionManager() = default;

FCollisionManager::~FCollisionManager() = default;

/*****************************/
/** Layer Collision Setting **/
/*****************************/

/**
 * @brief 레이어 간의 충돌 설정을 추가하는 함수
 */
void FCollisionManager::ActiveLayerCollision(UINT InLeftLayer, UINT InRightLayer)
{
	UINT Row = InLeftLayer;
	UINT Col = InRightLayer;

	if (Col < Row)
	{
		Row = InRightLayer;
		Col = InLeftLayer;
	}

	LayerCollisionMatrix[Row] |= (1 << Col);
}

/**
 * @brief 충돌 등록 Matrix를 초기화하는 함수
 */
void FCollisionManager::ClearCollisionBtwLayerSetting()
{
	memset(LayerCollisionMatrix, 0, sizeof(UINT) * MAX_LAYER);
}

/****************/
/** Main Logic **/
/****************/

/**
 * @brief 전체 충돌 처리 로직의 처리 함수
 * Create BVH : Bounding Volume Hirachy 구축, 이 과정에서 Level의 Active Object를 전부 가져와서 기록해둔다
 * -> Raycast Process : Raycast는 Collision 중 처리 방식이 상이하여 우선 처리한다, 개별 Broad - Narrow 처리를 통해 충돌쌍을 기록한다
 * -> Broad Phase : Main Collision의 Broad Phase, 여기서 AABB 판정을 통해 Candidates를 한정 짓는다
 * -> Narrow Phase : Candidates에 대해서 Compute Shader 처리를 통해 정밀하게 판정을 진행하며 이 과정에서 충돌쌍을 기록한다
 * -> PostProcess Phase : 확정된 충돌쌍에 대한 상호작용을 모든 오브젝트를 순회하면서 일괄 처리한다
 * -> Clean Resource : 사용된 자원들을 정리한다
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
		// LOG_INFO("[Collision][Main] Current Level Not Started");
		return;
	}

	LOG_INFO("[Collision][Main] Collision Process Start");

	// Create BVH Structure
	// LOG_INFO("[Collision][Main] Create Bounding Volume Hirachy Start");
	CreateBVHTree();

	// Raycast Process
	// LOG_INFO("[Collision][Main] Raycast Process Start");
	RaycastProcess();

	// Check Collision
	// LOG_INFO("[Collision][Main] Broad Phase Start");
	BroadPhase();
	// LOG_INFO("[Collision][Main] Narrow Phase Start");
	NarrowPhase();

	// LOG_INFO("[Collision][Main] PostProcess Phase Start");
	CollisionPostProcess();

	// Reset Information
	CleanResource();

	LOG_INFO("[Collision][Main] Collision Process Done");
}



/**
 * @brief 모든 충돌 판정이 완료된 후 Tick에서 사용한 자원을 정리하는 함수
 */
void FCollisionManager::CleanResource()
{
	// Reset Information
	CandidatePairVector.clear();
	LevelActiveObjects.clear();
	FrameCollisionSet.clear();

	// Swap & Clean Buffer To Use In Next Tick
	// std::swap(FrameCollisionSet, PrevFrameCollisionSet);
	// FrameCollisionSet->clear();
}
