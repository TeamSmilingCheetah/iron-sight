#pragma once
#include "ColliderBase.h"

/**
 * @brief 구체 형태의 충돌체 컴포넌트
 *
 * @param Radius Collider의 반지름
 */
class FSphereCollider :
	public IColliderBase
{
private:
	Vec3 Offset;
	float Scale;
	Matrix WorldMatrix;

public:
	void Init() override;
	void FinalTick() override;

	[[nodiscard]] const AABB GetAABB() const override;

	void SaveComponent(FILE* InFile) override;
	void LoadComponent(FILE* InFile) override;

	// Getter & Setter
	[[nodiscard]] EColliderType GetColliderType() const override { return EColliderType::SphereCollider; }

	// Special Member Function
	FSphereCollider();
	FSphereCollider(const FSphereCollider& InOrigin);
	~FSphereCollider() override;
	CLONE(FSphereCollider);
};
