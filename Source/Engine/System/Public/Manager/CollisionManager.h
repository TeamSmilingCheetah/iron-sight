#pragma once
#include "Engine/System/Public/Rendering/Shader/CMeshCollisionCS.h"
#include "Engine/System/Public/Rendering/Shader/CRaycastCS.h"

struct FRayCollisionInfo;
struct FCollisionID;

using ColliderPairVariant = variant<
	pair<FMeshCollider*, FMeshCollider*>,
	pair<FMeshCollider*, FBoxCollider*>,
	pair<FBoxCollider*, FMeshCollider*>,
	pair<FSphereCollider*, FSphereCollider*>,
	pair<FSphereCollider*, FBoxCollider*>,
	pair<FBoxCollider*, FSphereCollider*>,
	pair<FSphereCollider*, FMeshCollider*>,
	pair<FMeshCollider*, FSphereCollider*>
>;

class FPlaneCollider;
class FBoxCollider;
class FSphereCollider;
class FRayCollider;

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
	// Level Status
	UINT LayerCollisionMatrix[MAX_LAYER];
	CLevel* CurrentLevel;

	// Colliders
	vector<IColliderBase*> StaticColliders;
	vector<IColliderBase*> DynamicColliders;
	vector<FRayCollider*> RayColliders;

	// BVH
	BVHNode* StaticBVHRoot = nullptr;
	BVHNode* DynamicBVHRoot = nullptr;

	// Collision Containers
	vector<FCollisionID> CollisionCandidates;
	vector<FRayCollisionInfo> RayCandidates;
	unordered_set<FCollisionID> PrevFrameCollisionSet;
	unordered_set<FCollisionID> FrameCollisionSet;

	// CS Task
	CMeshCollisionCS MeshCollisionCS;
	CRaycastCS RaycastCS;
	vector<Vec3> FrameAllVertices;
	vector<UINT> FrameAllIndices;

	vector<tCollisionTask> Tasks;
	vector<tRaycastTask> RaycastTasks;
	vector<ColliderPairVariant> TaskColliders;
	vector<pair<FRayCollider*, FMeshCollider*>> RaycastTaskColliders;
	unordered_map<const void*, FMeshBatchData> DataCache;

private:
	// Main Flow
	bool PreProcess();
	void CreateBVHs();
	void RaycastProcess();
	void BroadPhase();
	void NarrowPhase();
	void PostProcess();
	void CleanResource();

	// BVH Functions
	static void BuildBVH(const vector<IColliderBase*>& InColliders, BVHNode*& OutNode);
	static void DestroyBVH(BVHNode*& InRootNode);
	static BVHNode* BuildBVHRecursive(const vector<IColliderBase*>& InColliders, int InDepth);
	static void QueryBVH(const BVHNode* InNode, const IColliderBase* InCollider, vector<IColliderBase*>& OutCandidates);
	static void QueryBVH(const BVHNode* InNode, FRayCollider* InRay, vector<FRayCollisionInfo>& OutCandidates);

	// Raycast Functions
	void RaycastBroad();
	void RaycastNarrow();
	void AddRayShaderTask(FRayCollider* InRay, const FMeshCollider* InCollider);
	void ExecuteAndProcessRaycastCS();

	// Broad Phase Functions
	void GetCandidatesInBVH(const BVHNode* InTreeRootNode, unordered_set<FCollisionID>& InCandidateCheckSet);
	bool IsLayerCollided(const IColliderBase* InLeftCollider, const IColliderBase* InRightCollider) const;
	void AddCandidate(IColliderBase* InLeftCollider, IColliderBase* InRightCollider);

	// Narrow Phase Functions
	static bool IsNeedCSTask(const IColliderBase* InLeftObject, const IColliderBase* InRightObject);
	void AddShaderTask(IColliderBase* InLeftCollider, IColliderBase* InRightCollider);
	void CheckCollisionInCPU(IColliderBase* InLeftCollider, IColliderBase* InRightCollider);
	static bool IsCollision(const FPlaneCollider* InLeftCollider, const FPlaneCollider* InRightCollider);
	static bool IsCollision(const FBoxCollider* InLeftCollider, const FBoxCollider* InRightCollider);
	static bool IsCollision(const FSphereCollider* InLeftCollider, const FSphereCollider* InRightCollider);
	static bool IsCollision(const FSphereCollider* InSphereCollider, const FBoxCollider* InBoxCollider);
	static bool IsCollision(const FBoxCollider* InBoxCollider, const FSphereCollider* InSphereCollider);
	static bool IsCollision(const FSphereCollider* InSphereCollider, const FMeshCollider* InMeshCollider);
	static bool IsCollision(const FMeshCollider* InMeshCollider, const FSphereCollider* InSphereCollider);
	static Vec3 GetClosestPointOnTriangle(const Vec3& InPoint,
	                                      const Vec3& InVertexA, const Vec3& InVertexB, const Vec3& InVertexC);
	void ExecuteAndProcessCS();

	// PostProcess Functions
	void ExecuteOverlap();

	// Common Functions
	void ClearDynamicColliders();
	void ClearAllColliders();
	void ClearTasks();
	void ClearCollisionContainers();
	void ClearBVHs();
	void ClearContainersForNextFrame();

	FMeshBatchData GetOrAddBatchData(const FBoxCollider* InCollider);
	FMeshBatchData GetOrAddBatchData(const FMeshCollider* InCollider);

	bool IsInCondition(unordered_set<FCollisionID>& InCandidateCheckSet,
	                   IColliderBase* InLeftCollider, IColliderBase* InRightCollider) const;
	void AddFrameCollision(ColliderVariant InLeftVariant, ColliderVariant InRightVariant);

public:
	void Tick();
	void ActiveLayerCollision(UINT InLeftLayer, UINT InRightLayer);
	void ClearCollisionBtwLayerSetting();
	void RemoveCollision(ColliderVariant InLeftVariant, ColliderVariant InRightVariant);
	void ClearPreviousLevelInformation();
};
