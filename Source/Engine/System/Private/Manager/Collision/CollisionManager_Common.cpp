#include "pch.h"
#include "Engine/System/Public/Manager/CollisionManager.h"

#include "Engine/Runtime/Public/Component/Physics/Collider3D.h"

constexpr UINT Indices[36] = {
	0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6,
	0, 4, 5, 0, 5, 1, 1, 5, 6, 1, 6, 2,
	2, 6, 7, 2, 7, 3, 3, 7, 4, 3, 4, 0
};

constexpr Vec3 UnitCube[8] = {
	{-0.5f, 0.5f, -0.5f},
	{0.5f, 0.5f, -0.5f},
	{0.5f, -0.5f, -0.5f},
	{-0.5f, -0.5f, -0.5f},
	{-0.5f, 0.5f, 0.5f},
	{0.5f, 0.5f, 0.5f},
	{0.5f, -0.5f, 0.5f},
	{-0.5f, -0.5f, 0.5f},
};

/**
 * @brief Level 실행 중 가지고 있는 Collider 중 Static을 제외한 나머지를 초기화하는 함수
 */
void FCollisionManager::ClearDynamicColliders()
{
	DynamicColliders.clear();
	RayColliders.clear();
}

/**
 * @brief 가지고 있는 Collider를 Static 충돌체까지 전부 Clearing하는 함수
 */
void FCollisionManager::ClearAllColliders()
{
	StaticColliders.clear();
	DynamicColliders.clear();
	RayColliders.clear();
}

/**
 * @brief Task 관련 정보를 전부 초기화하는 함수
 */
void FCollisionManager::ClearTasks()
{
	RaycastTasks.clear();
	RaycastTaskColliders.clear();

	Tasks.clear();
	TaskColliders.clear();

	FrameAllVertices.clear();
	FrameAllIndices.clear();
	DataCache.clear();
}

/**
 * @brief 충돌쌍 관련 정보를 가지고 있는 멤버 변수를 초기화 하는 함수
 */
void FCollisionManager::ClearCollisionContainers()
{
	PrevFrameCollisionSet->clear();
	FrameCollisionSet->clear();
	CollisionCandidates.clear();
}

/**
 * @brief BVH 멤버 변수들을 초기화 하는 함수
 */
void FCollisionManager::ClearBVHs()
{
	DestroyBVH(StaticBVHRoot);
	DestroyBVH(DynamicBVHRoot);
}

/**
 * @brief 이번 프레임에 사용할 충돌쌍 컨테이너들을 정리하는 함수
 * CollisionSet은 다음 프레임의 PrevCollisionSet으로 현 PrevCollisionSet만 정리 대상이기 때문에 Swap하고 Clear 처리한다
 */
void FCollisionManager::ClearContainersForNextFrame()
{
	CollisionCandidates.clear();
	RayCandidates.clear();

	// Swap & Clean Buffer To Use In Next Tick
	swap(FrameCollisionSet, PrevFrameCollisionSet);
	FrameCollisionSet->clear();
}

/**
 * @brief Collider3D 정보를 Buffer에 추가하는 함수
 */
FMeshBatchData FCollisionManager::GetOrAddBatchData(const FCollider3D* InCollider)
{
	// 중복 생성 방지
	const void* ColliderKey = InCollider;
	if (DataCache.contains(ColliderKey))
	{
		return DataCache.at(ColliderKey);
	}

	FMeshBatchData NewData;
	NewData.VertexOffset = static_cast<UINT>(FrameAllVertices.size());
	NewData.IndexOffset = static_cast<UINT>(FrameAllIndices.size());

	Matrix WorldMatrix = InCollider->GetColliderWorldMat();
	Vec3 Vertices[8];
	for (int i = 0; i < 8; ++i)
	{
		Vertices[i] = XMVector3Transform(UnitCube[i], WorldMatrix);
	}

	// Add Data
	for (int i = 0; i < 8; ++i)
	{
		FrameAllVertices.push_back(Vertices[i]);
	}
	for (int i = 0; i < 36; ++i)
	{
		FrameAllIndices.push_back(Indices[i]);
	}
	NewData.TriangleCount = 12;

	// Cache에 등록
	DataCache[ColliderKey] = NewData;

	return NewData;
}

void FCollisionManager::AddFrameCollision(ColliderVariant InLeftVariant, ColliderVariant InRightVariant) const
{
	IColliderBase* LeftCollider = GetBaseFromVariant(InLeftVariant);
	IColliderBase* RightCollider = GetBaseFromVariant(InRightVariant);

	FrameCollisionSet->insert(FCollisionID(LeftCollider, RightCollider));
}
