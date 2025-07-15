#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Component/Physics/CMeshCollider.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"

using std::ranges::sort;

/*****************/
/** Broad Check **/
/*****************/

void FCollisionManager::CheckBroadPhase()
{
	// Level Load Check
	auto* CurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (!CurrentLevel)
	{
		LOG_INFO("[Collision][Broad] No Level Found");
		return;
	}

	// Object Vector에 Object 전체 추가
	vector<CGameObject*> AllObjects;
	CurrentLevel->GetAllActiveObjectsInLevel(AllObjects);

	// 중복 배제를 위한 Set 추가
	unordered_set<ULONGLONG> CandidateSet;

	for (CGameObject* Object : AllObjects)
	{
		// 충돌 후보 선별
		vector<CGameObject*> Candidates;
		QueryBVH(MBVHRootNode, Object, Candidates);

		UINT ID_A = Object->GetID();

		for (CGameObject* Candidate : Candidates)
		{
			// Layer 조건 우선 배제
			if (MLayerCollisionMatrix[Object->GetLayerIdx()] & (1 << Candidate->GetLayerIdx()))
			{
				UINT ID_B = Candidate->GetID();

				// 고유 충돌 ID 생성
				if (ID_A > ID_B)
				{
					std::swap(ID_A, ID_B);
				}

				// 중복되지 않은 경우에만 세부 충돌 판정
				if (CandidateSet.insert(COLLISION_ID(ID_A, ID_B).ID).second)
				{
					AddCandidate(Object, Candidate);
				}
			}
		}
	}
}

/******************************/
/** Redirect & Add Candidate **/
/******************************/

/**
 * @brief 충돌 가능성이 보이는 오브젝트 쌍을 후보군 벡터에 올리는 함수
 *
 * @param PLeftObject Object 1
 * @param PRightObject Object 2
 */
void FCollisionManager::AddCandidate(CGameObject* PLeftObject, CGameObject* PRightObject)
{
	MCandidatePairVector.emplace_back(PLeftObject, PRightObject);
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
	vector<CGameObject*> sorted = PObjects;
	sort(sorted,
	     [Axis](const CGameObject* PObjectA, const CGameObject* PObjectB)
	     {
		     // Collider를 고려한 Sorting
		     if (PObjectA->Collider3D() && PObjectB->Collider3D())
		     {
			     return PObjectA->Collider3D()->GetColliderAABB().Center()[Axis]
				     < PObjectB->Collider3D()->GetColliderAABB().Center()[Axis];
		     }
		     if (PObjectA->Collider3D())
		     {
			     return PObjectA->Collider3D()->GetColliderAABB().Center()[Axis] < PObjectB->GetAABB().Center()[Axis];
		     }
		     if (PObjectB->Collider3D())
		     {
			     return PObjectA->GetAABB().Center()[Axis] < PObjectB->Collider3D()->GetColliderAABB().Center()[Axis];
		     }
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
 * @brief 기존에 BVH가 존재했다면 BVH를 제거하는 함수
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
 * @param PObject [IN] Intersect 판정이 필요한 AABB를 가진 오브젝트
 * @param PCandidates [OUT] AABB 충돌이 확정된 잠재적 충돌 의심군
 */
void FCollisionManager::QueryBVH(const BVHNode* PNode, const CGameObject* PObject, vector<CGameObject*>& PCandidates)
{
	if (!PNode)
	{
		LOG_ERROR("[Collision][Broad] Query 처리 중 BVHNode가 존재하지 않음");
		return;
	}

	// 3D Collider가 존재하는 경우 3D 충돌체 우선
	if (PObject->Collider3D())
	{
		// AABB가 겹치지 않는다면 탐색할 이유가 없으므로 그대로 반환
		if (!PNode->Bounds.Intersects(PObject->Collider3D()->GetColliderAABB()))
		{
			return;
		}

		// 만약 Leaf라면 해당 범위에 남은 오브젝트가 한정되어 있음
		if (PNode->IsLeaf())
		{
			for (auto* OtherObject : PNode->Objects)
			{
				// 다른 오브젝트도 3D Collider인 경우
				if (OtherObject->Collider3D())
				{
					if (PObject->Collider3D()->GetColliderAABB().Intersects(OtherObject->Collider3D()->GetColliderAABB()))
					{
						PCandidates.push_back(OtherObject);
					}
				}
				else
				{
					if (PObject->Collider3D()->GetColliderAABB().Intersects(OtherObject->GetAABB()))
					{
						PCandidates.push_back(OtherObject);
					}
				}
			}
		}
		// Leaf가 아니라면 Devide & Conquer로 재귀적 쿼리 처리
		else
		{
			QueryBVH(PNode->Left, PObject, PCandidates);
			QueryBVH(PNode->Right, PObject, PCandidates);
		}
	}
	// 3D Collider가 없는 일반 충돌 케이스
	else
	{
		// AABB가 겹치지 않는다면 탐색할 이유가 없으므로 그대로 반환
		if (!PNode->Bounds.Intersects(PObject->GetAABB()))
		{
			return;
		}

		// 만약 Leaf라면 해당 범위에 남은 오브젝트가 한정되어 있음
		if (PNode->IsLeaf())
		{
			for (auto* OtherObject : PNode->Objects)
			{
				// 다른 오브젝트가 3D Collider인 경우
				if (OtherObject->Collider3D())
				{
					if (PObject->GetAABB().Intersects(OtherObject->Collider3D()->GetColliderAABB()))
					{
						PCandidates.push_back(OtherObject);
					}
				}
				else
				{
					if (OtherObject->GetAABB().Intersects(PObject->GetAABB()))
					{
						PCandidates.push_back(OtherObject);
					}
				}
			}
		}
		// Leaf가 아니라면 Devide & Conquer로 재귀적 쿼리 처리
		else
		{
			QueryBVH(PNode->Left, PObject, PCandidates);
			QueryBVH(PNode->Right, PObject, PCandidates);
		}
	}
}
