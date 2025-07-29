#pragma once
#include "Engine/System/Public/Rendering/Shader/CMeshCollisionCS.h"
#include "Engine/System/Public/Rendering/Shader/CRaycastCS.h"

using ColliderPairVariant = variant<
	pair<FMeshCollider*, FMeshCollider*>,
	pair<FMeshCollider*, FCollider3D*>,
	pair<FCollider3D*, FMeshCollider*>
>;

struct FCollisionID
{
	IColliderBase* Left;
	IColliderBase* Right;

	FCollisionID(IColliderBase* InLeftCollider, IColliderBase* InRightCollider)
	{
		if (InLeftCollider > InRightCollider)
		{
			std::swap(InLeftCollider, InRightCollider);
		}

		Left = InLeftCollider;
		Right = InRightCollider;
	}

	auto operator<=>(const FCollisionID& InOther) const = default;
};

namespace std
{
	template <>
	struct hash<FCollisionID>
	{
		size_t operator()(const FCollisionID& InCollisionID) const noexcept
		{
			const size_t hash1 = hash<IColliderBase*>()(InCollisionID.Left);
			const size_t hash2 = hash<IColliderBase*>()(InCollisionID.Right);

			// Use hash_combine Method In Boost Library
			// 피보나치 해싱
			return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
		}
	};
}

struct MeshBatchData
{
	UINT VertexOffset;
	UINT IndexOffset;
	UINT TriCount;
};

class FCollider2D;
class FCollider3D;
class FColliderRay;
class FLandscape;
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
class FCollisionManager :
	public singleton<FCollisionManager>
{
	SINGLE(FCollisionManager)

private:
	UINT LayerCollisionMatrix[MAX_LAYER];
	CLevel* CurrentLevel;

	// Colliders
	vector<IColliderBase*> StaticColliders;
	vector<IColliderBase*> DynamicColliders;
	vector<FColliderRay*> RayColliders;

	// BVH
	BVHNode* StaticBVHRoot = nullptr;
	BVHNode* DynamicBVHRoot = nullptr;

	// Collision Collector
	vector<FCollisionID> CollisionCandidates;
	unordered_set<FCollisionID>* PrevFrameCollisionSet;
	unordered_set<FCollisionID>* FrameCollisionSet;

	// CS Task
	CMeshCollisionCS MeshCollisionCS;
	CRaycastCS RaycastCS;
	vector<Vec3> FrameAllVertices;
	vector<UINT> FrameAllIndices;

	vector<tRaycastTask> RaycastTasks;
	vector<pair<FColliderRay*, FMeshCollider*>> RaycastColliders;
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
	bool PreProcess();
	void CreateBVHTree();
	void RaycastProcess();
	void BroadPhase();
	void NarrowPhase();
	void CollisionPostProcess() const;
	void CleanResource();

	// BVH Function
	static void BuildBVH(const vector<IColliderBase*>& InColliders, BVHNode*& OutNode);
	static void DestroyBVH(BVHNode*& InRootNode);
	static BVHNode* BuildBVHRecursive(const vector<IColliderBase*>& InColliders, int InDepth);
	void GetCandidatesInBVHTree(const BVHNode* InTreeRootNode, unordered_set<FCollisionID>& InCandidateCheckSet);

	// Raycast Function
	static void QueryBVH(const BVHNode* InNode, const FColliderRay* InRay,
	                     vector<FRayColliderInfo>& OutIntersectVector);
	MeshBatchData GetOrAddRaycastBatchData(const FMeshCollider* InCollider);
	void AddRayShaderTask(FColliderRay* InRay, const FMeshCollider* InCollider);
	void ExecuteAndProcessRaycastCS();

	// Broad Phase Function
	static void QueryBVH(const BVHNode* InNode, const IColliderBase* InCollider, vector<IColliderBase*>& OutCandidates);
	bool IsInCondition(unordered_set<FCollisionID>& InCandidateCheckSet,
	                   IColliderBase* InLeftCollider, IColliderBase* InRightCollider) const;
	bool IsLayerCollided(const IColliderBase* InLeftCollider, const IColliderBase* InRightCollider) const;
	void AddCandidate(IColliderBase* InLeftCollider, IColliderBase* InRightCollider);

	// Narrow Phase Function
	void CheckPair(IColliderBase* InLeftCollider, IColliderBase* InRightCollider);
	void AddFrameCollision(ColliderVariant InLeftVariant, ColliderVariant InRightVariant) const;

	// Narrow CPU Task
	static bool IsCollision(const FCollider2D* InLeftCollider, const FCollider2D* InRightCollider);
	static bool IsCollision(const FCollider3D* InLeftCollider, const FCollider3D* InRightCollider);

	// Narrow GPU Task
	static bool NeedComputeShader(const IColliderBase* InLeftObject, const IColliderBase* InRightObject);
	MeshBatchData GetOrAddBatchData(const FCollider3D* InCollider);
	MeshBatchData GetOrAddBatchData(const FMeshCollider* InCollider);
	void AddShaderTask(IColliderBase* InLeftCollider, IColliderBase* InRightCollider);
	void ExecuteAndProcessCS();

	// Collision PostProcess
	void ExecuteOverlap() const;

public:
	void Tick();
	void ActiveLayerCollision(UINT InLeftLayer, UINT InRightLayer);
	void ClearCollisionBtwLayerSetting();
	void ClearPreviousLevelInformation();
};
