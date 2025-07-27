#pragma once
#include "ColliderBase.h"

/**
 * @brief mesh에 충돌체 성질을 부여하는 컴포넌트
 *
 * @param MMeshPtr 충돌의 기준이 될 mesh
 * @param MOverlapCount 현재 Overlap 중첩 횟수를 기록
 * @param MCollisionNormal mesh 충돌 시 충돌한 면의 법선 벡터
 * @param MPenetrationDepth mesh 충돌 시 침투 깊이
 */
class FMeshCollider :
	public IColliderBase
{
private:
	Ptr<CMesh> MMeshPtr;

public:
	void FinalTick() override;
	void Init() override;
	const AABB GetAABB() const override;

	void SaveComponent(FILE* InFile) override;
	void LoadComponent(FILE* InFile) override;

	// Getter & Setter
	EColliderType GetColliderType() const override { return EColliderType::MeshCollider; }
	Ptr<CMesh> GetMesh() const { return MMeshPtr; }

	void SetMesh(CMesh* PMesh) { MMeshPtr = PMesh; }

	// Special Member Function
	CLONE(FMeshCollider);
	FMeshCollider();
	FMeshCollider(const FMeshCollider& POrigin);
	~FMeshCollider() override;
};
