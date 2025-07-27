#pragma once

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/System/Public/Rendering/Shader/CMeshCollisionCS.h"
#include "Engine/System/Public/Rendering/Shader/CRaycastCS.h"

using ColliderPairVariant = variant<
	pair<CMeshCollider*, CMeshCollider*>,
	pair<CMeshCollider*, FCollider3D*>,
	pair<FCollider3D*, CMeshCollider*>
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
		if (PLeft > PRight)
		{
			std::swap(PLeft, PRight);
		}

		Left = PLeft;
		Right = PRight;
	}

	COLLISION_ID(ULONGLONG PCollisionID)
	{
		ID = PCollisionID;
	}
};

class FCollider2D;
class FCollider3D;
class CColliderRay;
class CLandScape;
class CMeshCollider;

/**
 * @brief 충돌을 관리하는 매니저 클래스
 *
 * @param MLayerCollisionMatrix 각 레이어 간의 충돌 검사 여부를 저장해둔 Matrix
 * @param MColllisionMap 충돌한 오브젝트들의 ID와 충돌 상태를 저장해둔 Map
 * @param MLevelActiveObjects 현재 Level의 전체 Active Object를 받아온 Vector
 * @param MMeshCollisionCS Mesh 충돌 판정 처리를 위한 Compute Shader
 * @param MCandidatePairVector Narrow 판정 처리를 위한 가능성 Pair를 모아두는 Vector
 * @param MFrameTasks 한 프레임 동안 GPU에서 처리할 모든 충돌 검사 작업 목록
 * @param MFrameTaskColliders FrameTasks의 각 충돌 작업에 해당하는 객체 포인터 쌍을 저장
 * @param MFrameAllVertices CS 처리 대상 충돌체들의 World Vertex 데이터를 하나로 합친 Buffer
 * @param MFrameAllIndices CS 처리 대상 충돌체들의 World Index 데이터를 하나로 합친 Buffer
 * @param MFrameDataCache 동일한 충돌체의 데이터가 중복으로 추가되는 것을 방지하기 위한 Cache
 */
class CollisionManager :
	public singleton<CollisionManager>
{
	SINGLE(CollisionManager)

private:
	struct MeshBatchData
	{
		UINT VertexOffset;
		UINT IndexOffset;
		UINT TriCount;
	};

	UINT LayerCollisionMatrix[MAX_LAYER];
	map<ULONGLONG, bool> ColllisionMap;
	vector<CGameObject*> LevelActiveObjects;
	set<ULONGLONG> FrameCollisionSet;
	CMeshCollisionCS MeshCollisionCS;
	CRaycastCS RaycastCS;
	vector<pair<const CGameObject*, const CGameObject*>> CandidatePairVector;
	BVHNode* BVHRootNode = nullptr;

	vector<Vec3> FrameAllVertices;
	vector<UINT> FrameAllIndices;

	vector<tRaycastTask> RaycastTasks;
	vector<pair<CColliderRay*, CMeshCollider*>> RaycastColliders;
	map<const void*, MeshBatchData> RaycastDataCache;

	vector<tCollisionTask> Tasks;
	vector<ColliderPairVariant> TaskColliders;
	map<const void*, MeshBatchData> DataCache;

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
	void BuildBVH(const vector<CGameObject*>& InObjects);
	void DestroyBVH();
	static BVHNode* BuildBVHRecursive(const vector<CGameObject*>& InObjects, int InDepth = 0);

	// Raycast Function
	static void QueryBVH(const BVHNode* InNode, const CColliderRay* InRay, vector<RayColliderInfo>& OutIntersectVector);
	MeshBatchData GetOrAddRaycastBatchData(const CMeshCollider* InCollider);
	void AddRayShaderTask(CColliderRay* InRay, const CGameObject* InObject);
	void ExecuteAndProcessRaycastCS();

	// Broad Phase Function
	static void QueryBVH(const BVHNode* InNode, const CGameObject* InObject, vector<CGameObject*>& OutCandidates);
	void AddCandidate(const CGameObject* InLeftObject, const CGameObject* InRightObject);

	// Narrow Phase Function
	void CheckPair(const CGameObject* InRightObject, const CGameObject* InLeftObject);
	void AddFrameCollision(ColliderVariant InLeftCollider, ColliderVariant InRightCollider);

	// Narrow CPU Task
	static bool IsCollision(const FCollider2D* InLeftCollider, const FCollider2D* InRightCollider);
	static bool IsCollision(const FCollider3D* InLeftCollider, const FCollider3D* InRightCollider);
	static bool IsCollision(const FCollider3D* InLeftCollider, const CLandScape* InRightCollider);

	// Narrow GPU Task
	static bool NeedComputeShader(const CGameObject* InLeftObject, const CGameObject* InRightObject);
	MeshBatchData GetOrAddBatchData(const FCollider3D* InCollider);
	MeshBatchData GetOrAddBatchData(const CMeshCollider* InCollider);
	void AddShaderTask(const CGameObject* InLeftObject, const CGameObject* InRightObject);
	void ExecuteAndProcessCS();

	// Collision PostProcess
	void CollisionsInLayer(UINT InLayerIndex);
	void CollisionBtwLayer(UINT InLeftLayerIndex, UINT InRightLayerIndex);
	void ExecuteOverlap(ColliderVariant InLeftCollider, ColliderVariant InRightCollider);

public:
	void Tick();
	void ActiveLayerCollision(UINT InLeftLayer, UINT InRightLayer);
	void ClearCollisionBtwLayerSetting();
};
