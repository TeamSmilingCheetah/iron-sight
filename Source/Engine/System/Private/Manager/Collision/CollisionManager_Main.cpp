#include "pch.h"
#include "Engine/System/Public/Manager/CollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Runtime/Public/Component/Physics/BoxCollider.h"
#include "Runtime/Public/Component/Physics/ColliderBase.h"
#include "Runtime/Public/Component/Physics/MeshCollider.h"

FCollisionManager::FCollisionManager() = default;

FCollisionManager::~FCollisionManager()
{
	ClearBVHs();
}

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

/**
 * @brief 특정 충돌쌍을 외부에서 제거하기 위한 함수
 * @param InLeftVariant Collider 1
 * @param InRightVariant Collider 2
 */
void FCollisionManager::RemoveCollision(ColliderVariant InLeftVariant, ColliderVariant InRightVariant)
{
	IColliderBase* LeftCollider =GetBaseFromVariant(InLeftVariant);
	IColliderBase* RightCollider =GetBaseFromVariant(InRightVariant);

	assert(LeftCollider && RightCollider);

	FCollisionID CollisionID(LeftCollider, RightCollider);
	FrameCollisionSet.erase(CollisionID);
	PrevFrameCollisionSet.erase(CollisionID);
}

/**
 * @brief 새로운 Level 시작 전 기존 Level 기준으로 들고 있던 정보들을 전부 제거하는 함수
 */
void FCollisionManager::ClearPreviousLevelInformation()
{
	// Clear Information
	ClearAllColliders();
	ClearCollisionContainers();

	// Clear Tasks
	ClearTasks();

	// Clear BVH
	ClearBVHs();
}

/****************/
/** Main Logic **/
/****************/

/**
 * @brief 전체 충돌 처리 로직의 처리 함수
 * PreProcess : 충돌 로직 진입 유무 등을 판단하고, 로직에 필요한 자원들을 미리 세팅한다
 * -> Create BVHs : Bounding Volume Hirachy 구축, 이 과정에서 Level의 Active Object를 전부 가져와서 세팅한다
 * -> Raycast Process : Raycast는 우선처리 대상으로, 내부적으로 Broad - Narrow 처리를 통해 충돌쌍을 기록하여 넘긴다
 * -> Broad Phase : Main Collision의 Broad Phase, 여기서 AABB 판정을 통해 Candidates를 한정 짓는다
 * -> Narrow Phase : Candidates에 대해서 Compute Shader 처리를 통해 정밀하게 판정을 진행하며 이 과정에서 충돌쌍을 기록한다
 * -> PostProcess Phase : 확정된 충돌쌍에 대한 상호작용을 모든 오브젝트를 순회하면서 일괄 처리한다
 * -> Clean Resource : 사용된 자원들을 정리한다
 */
void FCollisionManager::Tick()
{
	// Early Return
	if (!PreProcess())
	{
		CleanResource();
		return;
	}

	// LOG_INFO("[Collision][Main] Collision Process Start");
	CreateBVHs();
	RaycastProcess();
	BroadPhase();
	NarrowPhase();
	PostProcess();
	CleanResource();
	// LOG_INFO("[Collision][Main] Collision Process Done");
}

/**
 * @brief CollisionManager가 충돌 연산을 처리하기 전에 관련 Resource를 받아서 정리하는 함수
 * Resource 처리 중 Tick 처리하지 않을 조건들을 발견하면 Tick 처리하지 않고 종료하도록 Trigger를 남긴다
 * @return 이후 Process를 실행할지 여부
 */
bool FCollisionManager::PreProcess()
{
	CurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	// Early Return (이후로 Current Level 없는 상황은 발생하지 않음)
	if (!CurrentLevel)
	{
		LOG_INFO("[Collision][Main] No Current Level");
		return false;
	}

	// TODO(KHJ): 이 Log 너무 자주 호출됨. 빈번한 로그는 적당히 걸러내는 기능이 필요함
	if (CurrentLevel->GetState() == LEVEL_STATE::STOP)
	{
		// LOG_INFO("[Collision][Main] Current Level Not Started");
		return false;
	}

	// Static Collider는 Level 변경 직후에만 제공 받고, 그 이후로는 따로 리셋하여 다시 쌓지 않는다
	if (StaticColliders.empty())
	{
		CurrentLevel->GetAllCollidersInLevel(StaticColliders, DynamicColliders, RayColliders);
	}
	else
	{
		CurrentLevel->GetDynamicCollidersInLevel(DynamicColliders, RayColliders);
	}

	return true;
}

/**
 * @brief 충돌 판정을 위한 BVH 구조를 구축하는 함수
 * 이미 StaticBVH가 구축이 되어 있다면 따로 구축하지 않으며, DynamicBVH는 매 Tick마다 구축해야 한다
 */
void FCollisionManager::CreateBVHs()
{
	// XXX(KHJ): 혹시 정적인 충돌체가 사라지는 케이스가 존재한다면 해당 케이스에는 재구축을 해야 한다
	// XXX(KHJ): 그러나 그런 케이스라면 그냥 처음부터 동적 충돌체로 잡는 게 나을 수 있음
	if (!StaticBVHRoot)
	{
		BuildBVH(StaticColliders, StaticBVHRoot);
	}

	BuildBVH(DynamicColliders, DynamicBVHRoot);

	// LOG_INFO_F("[Collision][VBH] Total Static Colliders: {}", StaticColliders.size());
	// LOG_INFO_F("[Collision][VBH] Total Dynamic Colliders: {}", DynamicColliders.size());
}

/**
 * @brief 레벨 내 모든 Ray Collider와 충돌 가능한 오브젝트에 대해 Raycast를 처리하는 함수
 * 다른 충돌체처럼 Ray를 대상으로 동일하게 Broad - Narrow 구조로 내부에서 처리 후 CS Task까지 완료한다
 */
void FCollisionManager::RaycastProcess()
{
	RaycastBroad();
	RaycastNarrow();
}

/**
 * @brief 모든 Dynamic Collider에 대해, Narrow Phase로 진행할 충돌쌍을 골라내는 과정
 * Static BVH와 Dynamic BVH에 각각 검증하는 절차를 진행함
 */
void FCollisionManager::BroadPhase()
{
	// 중복 배제를 위한 Set 생성
	unordered_set<FCollisionID> CandidateSet;

	GetCandidatesInBVH(StaticBVHRoot, CandidateSet);
	GetCandidatesInBVH(DynamicBVHRoot, CandidateSet);
}

/**
 * @brief 충돌 가능성이 있는 오브젝트 쌍에 대해 충돌 판정을 처리하는 함수
 */
void FCollisionManager::NarrowPhase()
{
	// CPU와 GPU 처리 대상을 분리
	for (const FCollisionID ID : CollisionCandidates)
	{
		if (IsNeedCSTask(ID.Left, ID.Right))
		{
			// CS 처리가 필요한 경우 CS Task에만 달고 나감
			AddShaderTask(ID.Left, ID.Right);
		}
		else
		{
			// CS 처리 대상이 아닐 경우, 여기서 충돌 처리 마무리
			CheckCollisionInCPU(ID.Left, ID.Right);
		}
	}

	// Process Delayed Batch Process
	ExecuteAndProcessCS();
}

void FCollisionManager::PostProcess()
{
	ExecuteOverlap();
}

/**
 * @brief 모든 충돌 판정이 완료된 후 Tick에서 사용한 자원을 정리하는 함수
 */
void FCollisionManager::CleanResource()
{
	// Reset Information
	ClearContainersForNextFrame();
	ClearDynamicColliders();
	ClearTasks();
}
