#pragma once
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/System/Public/Rendering/Shader/CMeshCollisionCS.h"
#include "Engine/System/Public/Rendering/Shader/CRaycastCS.h"

using ColliderPairVariant = variant<
	pair<CMeshCollider*, CMeshCollider*>,
	pair<CMeshCollider*, CCollider3D*>,
	pair<CCollider3D*, CMeshCollider*>
>;

union COLLISION_ID
{
	struct
	{
		UINT Left;
		UINT Right;
	};

	ULONGLONG ID;

	COLLISION_ID(UINT PLeft, UINT PRight)
	{
		Left = PLeft;
		Right = PRight;
	}

	COLLISION_ID(ULONGLONG PCollisionID)
	{
		ID = PCollisionID;
	}
};

class CCollider2D;
class CCollider3D;
class CColliderRay;
class CLandScape;
class CMeshCollider;

/**
 * @brief 충돌을 관리하는 매니저 클래스
 *
 * @param MLayerCollisionMatrix 각 레이어 간의 충돌 검사 여부를 저장해둔 Matrix
 * @param MColllisionInfoMap 충돌한 오브젝트들의 ID와 충돌 상태를 저장해둔 Map
 * @param MMeshCollisionCS Mesh 충돌 판정 처리를 위한 Compute Shader
 * @param MCandidatePairVector Narrow 판정 처리를 위한 가능성 Pair를 모아두는 Vector
 * @param MFrameTasks 한 프레임 동안 GPU에서 처리할 모든 충돌 검사 작업 목록
 * @param MFrameTaskColliders FrameTasks의 각 충돌 작업에 해당하는 객체 포인터 쌍을 저장
 * @param MFrameAllVertices CS 처리 대상 충돌체들의 World Vertex 데이터를 하나로 합친 Buffer
 * @param MFrameAllIndices CS 처리 대상 충돌체들의 World Index 데이터를 하나로 합친 Buffer
 * @param MFrameDataCache 동일한 충돌체의 데이터가 중복으로 추가되는 것을 방지하기 위한 Cache
 */
class FCollisionManager :
	public singleton<FCollisionManager>
{
	SINGLE(FCollisionManager);

private:
	struct MeshBatchData
	{
		UINT VertexOffset;
		UINT IndexOffset;
		UINT TriCount;
	};

	UINT MLayerCollisionMatrix[MAX_LAYER];
	map<ULONGLONG, bool> MColllisionInfoMap;
	set<ULONGLONG> MFrameCollisionSet;
	CMeshCollisionCS MMeshCollisionCS;
	CRaycastCS MRaycastCS;
	vector<pair<const CGameObject*, const CGameObject*>> MCandidatePairVector;
	BVHNode* MBVHRootNode = nullptr;

	vector<Vec3> MFrameAllVertices;
	vector<UINT> MFrameAllIndices;

	vector<tCollisionTask> MTasks;
	vector<ColliderPairVariant> MTaskColliders;
	map<const void*, MeshBatchData> MDataCache;

	vector<tRaycastTask> MRaycastTasks;
	vector<pair<CColliderRay*, CMeshCollider*>> MRaycastColliders;
	map<const void*, MeshBatchData> MRaycastDataCache;

public:
	struct SimpleVtx
	{
		Vec3 pos;
	};

	struct SimpleIdx
	{
		uint32_t x, y, z;
	};

private:
	// Main Flow
	void CreateBVHTree();
	void RaycastProcess();
	void BroadPhase();
	void NarrowPhase();
	void CollisionPostProcess();
	void CleanResource();

	// BVH Function
	void BuildBVH(const vector<CGameObject*>& PObjects);
	void DestroyBVH();
	static BVHNode* BuildBVHRecursive(const vector<CGameObject*>& PObjects, int PDepth = 0);

	// Raycast Function
	static void QueryBVH(const BVHNode* PNode, const CColliderRay* PRay, vector<RayColliderInfo>& PIntersectVector);
	MeshBatchData GetOrAddRaycastBatchData(CMeshCollider* pCollider);
	void AddRayShaderTask(CColliderRay* PRay, const CGameObject* PObject);
	void ExecuteAndProcessRaycastCS();

	// Broad Phase Function
	static void QueryBVH(const BVHNode* PNode, const CGameObject* PObject, vector<CGameObject*>& PCandidates);
	void AddCandidate(const CGameObject* PLeftObject, const CGameObject* PRightObject);

	// Narrow Phase Function
	void CheckPair(const CGameObject* PRightObject, const CGameObject* PLeftObject);
	void AddFrameCollisionInfo(CGameObject* PObject1, CGameObject* PObject2);

	// Narrow CPU Task
	static bool IsCollision(const CCollider2D* PLeftCollider, const CCollider2D* PRightCollider);
	static bool IsCollision(const CCollider3D* PLeftCollider, const CCollider3D* PRightCollider);
	static bool IsCollision(const CCollider3D* PLeftCollider, const CLandScape* PRightCollider);

	// Narrow GPU Task
	static bool NeedComputeShader(const CGameObject* PLeftObject, const CGameObject* PRightObject);
	MeshBatchData GetOrAddBatchData(const CCollider3D* PCollider);
	MeshBatchData GetOrAddBatchData(const CMeshCollider* PCollider);
	void AddShaderTask(const CGameObject* PLeftObject, const CGameObject* PRightObject);
	void ExecuteAndProcessCS();

	// Collision Apply
	template <typename T1, typename T2>
	void ProcessCollision(T1* PLeftCollider, T2* PRightCollider);
	template <typename T1, typename T2>
	static void ProcessEndOverlap(T1* PLeftCollider, T2* PRightCollider);

public:
	void Tick();
	void ActiveLayerCollision(UINT PLeft, UINT PRight);
	void ClearCollisionBtwLayerSetting();
};

/**
 * @brief 충돌체 간의 충돌 상태에 따라 오브젝트의 이후 동작을 유도하는 함수
 *
 * @tparam T1 Collider Type 1
 * @tparam T2 Collider Type 2
 * @param PLeftCollider Collider 1
 * @param PRightCollider Collider 2
 */
template <typename T1, typename T2>
void FCollisionManager::ProcessCollision(T1* PLeftCollider, T2* PRightCollider)
{
	// Set Collision ID
	COLLISION_ID CollisionID(PLeftCollider->GetID(), PRightCollider->GetID());

	// 기존 충돌 정보와 대조 후 Map에 없으면 추가
	auto CollisionPair = MColllisionInfoMap.find(CollisionID.ID);
	if (CollisionPair == MColllisionInfoMap.end())
	{
		MColllisionInfoMap.insert(make_pair(CollisionID.ID, false));
		CollisionPair = MColllisionInfoMap.find(CollisionID.ID);
	}

	CGameObject* LeftObject = PLeftCollider->GetOwner();
	CGameObject* RightObject = PRightCollider->GetOwner();

	bool IsDead = LeftObject->IsDead() || RightObject->IsDead();
	bool IsDeactive = !LeftObject->IsActive() || !RightObject->IsActive();
	bool IsLayerChanged = LeftObject->IsLayerMove() || RightObject->IsLayerMove();

	if (CollisionPair->second)
	{
		// EndOverlap
		if (IsDead || IsDeactive || IsLayerChanged)
		{
			PLeftCollider->EndOverlap(PRightCollider);
			PRightCollider->EndOverlap(PLeftCollider);
			CollisionPair->second = false;
		}
		// Overlap
		else
		{
			PLeftCollider->Overlap(PRightCollider);
			PRightCollider->Overlap(PLeftCollider);
		}
	}
	else
	{
		// BeginOverlap
		if (!IsDead && !IsDeactive)
		{
			PLeftCollider->BeginOverlap(PRightCollider);
			PRightCollider->BeginOverlap(PLeftCollider);
			CollisionPair->second = true;
		}
	}
}

/**
 * @brief 두 충돌체의 EndOverlap을 처리하는 함수
 * 충돌 과정 전반이 종료된 뒤 충돌이 종료됨이 감지되지 않은 쌍에 대해 충돌 종료 호출
 * @tparam T1 Collider Type 1
 * @tparam T2 Collider Type 2
 * @param PLeftCollider Collider 1
 * @param PRightCollider Collider 2
 */
template <typename T1, typename T2>
void FCollisionManager::ProcessEndOverlap(T1* PLeftCollider, T2* PRightCollider)
{
	PLeftCollider->EndOverlap(PRightCollider);
	PRightCollider->EndOverlap(PLeftCollider);
}
