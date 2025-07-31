#pragma once
#include "ColliderBase.h"

/**
 * @brief Mesh에 충돌체 성질을 부여하는 컴포넌트
 *
 * @param MeshPtr 충돌체 성질을 부여 받은 Mesh의 Pointer
 * @param ConvexHullMeshPtr Mesh를 기반으로 생성한 볼록 입체도형 Mesh
 */
class FMeshCollider :
	public IColliderBase
{
private:
	Ptr<CMesh> MeshPtr;
	Ptr<CMesh> ConvexHullMeshPtr;

private:
	void GenerateConvexHull();

public:
	void Init() override;
	void FinalTick() override;
	[[nodiscard]] const AABB GetAABB() const override;

	void SaveComponent(FILE* InFile) override;
	void LoadComponent(FILE* InFile) override;

	// Getter & Setter
	[[nodiscard]] EColliderType GetColliderType() const override { return EColliderType::MeshCollider; }
	[[nodiscard]] Ptr<CMesh> GetMesh() const { return !!ConvexHullMeshPtr.Get() ? ConvexHullMeshPtr : MeshPtr; }

	void SetMesh(CMesh* InMesh) { MeshPtr = InMesh; }

	// Special Member Function
	FMeshCollider();
	FMeshCollider(const FMeshCollider& InOrigin);
	~FMeshCollider() override;
	CLONE(FMeshCollider);
};
