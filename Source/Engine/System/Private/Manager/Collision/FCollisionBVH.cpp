#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Component/Physics/Collider3D.h"
#include "Runtime/Public/Component/Physics/ColliderRay.h"

using std::ranges::sort;

/*****************************/
/** Bounding Volume Hirachy **/
/*****************************/

/**
 * @brief BVH Tree를 구축하는 함수
 */
void CollisionManager::CreateBVHTree()
{
	CLevel* CurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	CurrentLevel->GetAllActiveObjectsInLevel(LevelActiveObjects);
	BuildBVH(LevelActiveObjects);

	// LOG_INFO_F("[Collision][VBH] Total Active Objects: {}", PObjects.size());
}

/**
 * @brief BVH 트리 구조를 구축하는 함수
 *
 * @param InObjects 전체 Game Object가 담긴 Vector
 * @param InDepth 재귀 Depth
 * @return 생성된 트리의 Root Node
 */
BVHNode* CollisionManager::BuildBVHRecursive(const vector<CGameObject*>& InObjects, int InDepth)
{
	if (InObjects.empty())
	{
		LOG_ERROR("[Collision][VBH] Tree를 작성하기 위한 Object Vector가 존재하지 않음");
		return nullptr;
	}

	if (!InDepth)
	{
		// LOG_INFO("[Collision][VBH] Bounding Volume Hirachy Build Start");
	}

	BVHNode* Node = new BVHNode();

	// 일단 오브젝트를 돌면서 현재 Node의 Bound를 늘림
	for (auto* Object : InObjects)
	{
		Node->Bounds.Expand(Object->GetCollisionAABB());
	}

	// Leaf에 도달한 상태라면 Node를 반환
	if (InObjects.size() <= 2)
	{
		Node->Objects = InObjects;
		return Node;
	}

	// Sorting By Longest Axis (By Heuristic)
	int Axis = Node->Bounds.LongestAxis();
	vector<CGameObject*> sorted = InObjects;
	sort(sorted,
	     [Axis](const CGameObject* PObjectA, const CGameObject* PObjectB)
	     {
		     return PObjectA->GetCollisionAABB().Center()[Axis] < PObjectB->GetCollisionAABB().Center()[Axis];
	     }
	);

	// Prepare Left, Right Node
	size_t Mid = sorted.size() / 2;
	vector<CGameObject*> LeftRangeVector(sorted.begin(), sorted.begin() + Mid);
	vector<CGameObject*> RightRangeVector(sorted.begin() + Mid, sorted.end());

	// 재귀적 함수 호출로 본인 Node 형성 후 Left, Right 노드에 등록
	Node->Left = BuildBVHRecursive(LeftRangeVector, InDepth + 1);
	Node->Right = BuildBVHRecursive(RightRangeVector, InDepth + 1);

	if (!InDepth)
	{
		// LOG_INFO("[Collision][VBH] Bounding Volume Hirachy Creation Complete");
	}

	// Node 반환
	return Node;
}

/**
 * @brief 기존 BVH를 제거하고 BVH를 재구축하는 함수
 *
 * @param InObjects 전체 오브젝트가 담긴 Vector
 */
void CollisionManager::BuildBVH(const vector<CGameObject*>& InObjects)
{
	DestroyBVH();
	BVHRootNode = BuildBVHRecursive(InObjects, 0);
}

/**
 * @brief 기존에 BVH가 존재했다면 BVH를 제거하는 함수
 */
void CollisionManager::DestroyBVH()
{
	delete BVHRootNode;
	BVHRootNode = nullptr;
	// LOG_INFO("[Collision][VBH] Bounding Volume Hirachy Destroy Complete");
}

/**
 * @brief Root부터 재귀적으로 충돌에 대해 탐색 처리를 진행하고, 결과를 제공한 Vector에 적재하는 함수
 * @param InNode [IN] 현재 Intersect 체크가 필요한 Node
 * @param InObject [IN] Intersect 판정이 필요한 AABB를 가진 오브젝트
 * @param OutCandidates [OUT] AABB 충돌이 확정된 잠재적 충돌 의심군
 */
void CollisionManager::QueryBVH(const BVHNode* InNode, const CGameObject* InObject, vector<CGameObject*>& OutCandidates)
{
	if (!InNode)
	{
		LOG_ERROR("[Collision][VBH] Query 처리 중 BVHNode가 존재하지 않음");
		return;
	}

	// AABB가 겹치지 않는다면 탐색할 이유가 없으므로 그대로 반환
	if (!InNode->Bounds.Intersects(InObject->GetCollisionAABB()))
	{
		return;
	}

	// 만약 Leaf라면 해당 범위에 남은 오브젝트가 한정되어 있음
	if (InNode->IsLeaf())
	{
		for (auto* OtherObject : InNode->Objects)
		{
			if (InObject->GetAABB().Intersects(OtherObject->GetCollisionAABB()))
			{
				OutCandidates.push_back(OtherObject);
			}
		}
	}

	// Leaf가 아니라면 Devide & Conquer로 재귀적 쿼리 처리
	else
	{
		QueryBVH(InNode->Left, InObject, OutCandidates);
		QueryBVH(InNode->Right, InObject, OutCandidates);
	}
}

/**
 * @brief BVH 트리를 탐색하며 Ray와의 충돌에서 가능성이 있는 객체 정보를 제공하는 함수
 * @param InNode [IN] 현재 Intersect 체크가 필요한 Node
 * @param InRay [IN] Node와 충돌 검사를 진행하는 Main 객체
 * @param OutIntersectVector [OUT] 충돌이 확정된 오브젝트들의 정보가 담길 Vector
 */
void CollisionManager::QueryBVH(const BVHNode* InNode, const FColliderRay* InRay,
                                 vector<FRayColliderInfo>& OutIntersectVector)
{
	if (!InNode || !InRay)
	{
		LOG_ERROR("[Collision][VBH] Query 처리 중 BVHNode 또는 Ray가 존재하지 않음");
		return;
	}

	tRay RayInfo;
	RayInfo.vStart = InRay->GetRayFinalPos();
	RayInfo.vDir = InRay->GetRayFinalDir();

	// 현재 Node의 AABB와 Ray가 교차하지 않으면 탐색 중단
	if (!InNode->Bounds.Intersects(RayInfo))
	{
		return;
	}

	// 리프 노드라면, 포함된 오브젝트에 대해 교차 검사 진행
	if (InNode->IsLeaf())
	{
		for (auto* Object : InNode->Objects)
		{
			// Owner와는 충돌하지 않도록 처리
			if (Object == InRay->GetOwner())
			{
				continue;
			}

			// 충돌한 경우, 충돌 정보 담아서 기록
			float TimeMin = 0.f;
			if (Object->GetCollisionAABB().Intersects(RayInfo, &TimeMin))
			{
				FRayColliderInfo ColliderInfo;
				ColliderInfo.RayObject = const_cast<FColliderRay*>(InRay);
				ColliderInfo.HitObject = Object;
				ColliderInfo.Length = TimeMin;
				OutIntersectVector.push_back(ColliderInfo);
			}
		}
	}

	// Leaf가 아니라면 Devide & Conquer로 재귀적 쿼리 처리
	else
	{
		QueryBVH(InNode->Left, InRay, OutIntersectVector);
		QueryBVH(InNode->Right, InRay, OutIntersectVector);
	}
}
