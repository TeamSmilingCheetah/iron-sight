#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

using std::ranges::sort;

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
	MCandidatePairVector.clear();
}

/*****************************/
/** Bounding Volume Hirachy **/
/*****************************/

/**
 * @brief BVH 트리 구조를 구축하는 함수
 *
 * @param PObjects 전체 Game Object가 담긴 Vector
 * @param PDepth 재귀 Depth
 * @return 생성된 트리의 Root Node
 */
BVHNode* FCollisionManager::BuildBVHRecursive(const vector<CGameObject*>& PObjects, int PDepth)
{
	if (PObjects.empty())
	{
		LOG_ERROR("[Collision] BVH Tree를 작성할 Object가 존재하지 않음");
		return nullptr;
	}

	if (!PDepth)
	{
		LOG_INFO("[Collision][Broad] Bounding Volume Hirachy 구축 시작");
	}
	BVHNode* Node = new BVHNode();

	// 일단 오브젝트를 돌면서 현재 Node의 Bound를 늘림
	for (auto* Object : PObjects)
	{
		Node->Bounds.Expand(Object->GetAABB());
	}

	// Leaf에 도달한 상태라면 Node를 반환
	if (PObjects.size() <= 2)
	{
		Node->Objects = PObjects;
		return Node;
	}

	// Sorting By Longest Axis (By Heuristic)
	int Axis = Node->Bounds.LongestAxis();
	auto sorted = PObjects;
	sort(sorted,
	     [Axis](const CGameObject* PObjectA, const CGameObject* PObjectB)
	     {
		     return PObjectA->GetAABB().Center()[Axis] < PObjectB->GetAABB().Center()[Axis];
	     }
	);

	// Prepare Left, Right Node
	size_t Mid = sorted.size() / 2;
	vector<CGameObject*> LeftRangeVector(sorted.begin(), sorted.begin() + Mid);
	vector<CGameObject*> RightRangeVector(sorted.begin() + Mid, sorted.end());

	// 재귀적 함수 호출로 본인 Node 형성 후 Left, Right 노드에 등록
	Node->Left = BuildBVHRecursive(LeftRangeVector, PDepth + 1);
	Node->Right = BuildBVHRecursive(RightRangeVector, PDepth + 1);

	if (!PDepth)
	{
		LOG_INFO("[Collision][Broad] Bounding Volume Hirachy 구축 완료");
	}

	// Node 반환
	return Node;
}

/**
 * @brief 기존 BVH를 제거하고 BVH를 재구축하는 함수
 *
 * @param PObjects 전체 오브젝트가 담긴 Vector
 */
void FCollisionManager::BuildBVH(const vector<CGameObject*>& PObjects)
{
	DestroyBVH();
	MBVHRootNode = BuildBVHRecursive(PObjects, 0);
}

/**
 * @brief BVH를 제거하는 함수
 */
void FCollisionManager::DestroyBVH()
{
	delete MBVHRootNode;
	MBVHRootNode = nullptr;
	LOG_INFO("[Collision][Broad] Bounding Volume Hirachy 제거 완료");
}

/**
 * @brief Root부터 재귀적으로 충돌에 대해 탐색 처리를 진행하고, 결과를 제공한 Vector에 적재하는 함수
 * @param PNode [IN] 현재 Intersect 체크가 필요한 Node
 * @param PQueryBound [IN] Intersect 판정이 필요한 범위
 * @param POutCandidates [OUT] AABB 충돌이 확정된 잠재적 충돌 의심군
 */
void FCollisionManager::QueryBVH(const BVHNode* PNode, const AABB& PQueryBound, vector<CGameObject*>& POutCandidates)
{
	if (!PNode)
	{
		LOG_ERROR("[Collision][Broad] Query 처리 중 BVHNode가 존재하지 않음");
		return;
	}

	// AABB가 겹치지 않는다면 탐색할 이유가 없으므로 그대로 반환
	if (!PNode->Bounds.Intersects(PQueryBound))
	{
		return;
	}

	// 만약 Leaf라면 해당 범위에 남은 오브젝트가 한정되어 있음
	if (PNode->IsLeaf())
	{
		for (auto* Object : PNode->Objects)
			if (Object->GetAABB().Intersects(PQueryBound))
			{
				POutCandidates.push_back(Object);
			}
	}
	// Leaf가 아니라면 Devide & Conquer로 재귀적 쿼리 처리
	else
	{
		QueryBVH(PNode->Left, PQueryBound, POutCandidates);
		QueryBVH(PNode->Right, PQueryBound, POutCandidates);
	}
}
