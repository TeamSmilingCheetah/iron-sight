#pragma once
#include "ColliderBase.h"

/**
 * @brief 구체 형태의 충돌체 컴포넌트
 *
 * @param Offset 충돌체의 Local 중심점
 * @param Scale 충돌체의 Local 반지름
 * @param WorldOffset 충돌체의 World 중심점
 * @param WorldScale 충돌체의 World 반지름
 * @param bHasIndependentScale 부모 오브젝트와 독립적인 사이즈인지 여부
 */
class FSphereCollider :
	public IColliderBase
{
private:
	Vec3 Offset;
	Vec3 WorldOffset;
	float Scale;
	float WorldScale;
	bool bHasIndependentScale;

public:
	void Init() override;
	void FinalTick() override;

	[[nodiscard]] const AABB GetAABB() const override;

	void SaveComponent(FILE* InFile) override;
	void LoadComponent(FILE* InFile) override;

	// Getter & Setter
	[[nodiscard]] EColliderType GetColliderType() const override { return EColliderType::SphereCollider; }

	void SetIndependent() { bHasIndependentScale = true; }

	void SetIndependent(float InScale)
	{
		bHasIndependentScale = true;
		Scale = InScale;
	}

	// Special Member Function
	FSphereCollider();
	FSphereCollider(const FSphereCollider& InOrigin);
	~FSphereCollider() override;
	CLONE(FSphereCollider);
};
