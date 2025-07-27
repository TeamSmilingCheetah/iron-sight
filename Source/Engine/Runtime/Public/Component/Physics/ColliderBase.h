#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

enum class EColliderType : UINT8
{
	Null,
	Collider2D,
	Collider3D,
	MeshCollider,
	ColliderRay,

	End,
};

/**
 * @brief 모든 Collider의 기본 클래스
 *
 * @param OverlapCount 현재 Overlap 상태인 다른 충돌체의 수
 * @param State 충돌체의 활성화 상태
 */
class IColliderBase : public CComponent
{
private:
	int OverlapCount;
	COLLIDER_STATE ColliderState;
	Vec3 CollisionNormal;
	float PenetrationDepth;

protected:
	// Only Derived Class Can Use Constructor Function
	IColliderBase(COMPONENT_TYPE InType);

public:
	void FinalTick() override = 0;
	void SaveComponent(FILE* InFile) override = 0;
	void LoadComponent(FILE* InFile) override = 0;
	virtual const AABB GetAABB() const = 0;
	virtual EColliderType GetColliderType() const = 0;

	// Getter & Setter
	COLLIDER_STATE GetColliderState() const { return ColliderState; }
	bool IsActive() const { return ColliderState == ACTIVE; }
	bool IsDeactive() const { return ColliderState == DEACTIVE; }
	bool IsSemiDeactive() const { return ColliderState == SEMIDEACTIVE; }
	bool IsOverlapped() const { return OverlapCount != 0; }
	Vec3 GetCollisionNormal() const { return CollisionNormal; }
	float GetPenetrationDepth() const { return PenetrationDepth; }

	void SetActivate() { ColliderState = ACTIVE; }
	void SetSemiDeactivate() { ColliderState = SEMIDEACTIVE; }
	void SetDeactive() { ColliderState = DEACTIVE; }
	void SetCollisionNormal(Vec3 InNormal) { CollisionNormal = InNormal; }
	void SetPenetrationDepth(float InDepth) { PenetrationDepth = InDepth; }
	void IncreaseOverlapCount() { ++OverlapCount; }
	void DecreaseOverlapCount() { --OverlapCount; }

	// Special Member Function
	~IColliderBase() override;
	IColliderBase(const IColliderBase& POrigin);
};

inline IColliderBase::IColliderBase(COMPONENT_TYPE InType)
	: CComponent(InType), OverlapCount(0), ColliderState(ACTIVE), PenetrationDepth(0)
{
}

inline IColliderBase::IColliderBase(const IColliderBase& POrigin)
	: CComponent(POrigin)
	  , OverlapCount(0)
	  , ColliderState(POrigin.ColliderState)
	  , PenetrationDepth(0)
{
}

inline IColliderBase::~IColliderBase() = default;
