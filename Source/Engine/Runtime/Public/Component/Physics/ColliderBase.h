#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

enum class EColliderType : UINT8
{
	NONE,
	Collider2D,
	Collider3D,
	MeshCollider,
	SphereCollider,
	ColliderRay,

	END = 0xFF,
};

/**
 * @brief 모든 Collider의 기본 클래스
 *
 * @param OverlapCount 현재 Overlap 상태인 다른 충돌체의 수
 * @param ColliderState 충돌체의 활성화 상태
 * @param CollsionNormal 충돌 이후 충돌면의 노멀 벡터를 기록하는 변수
 * @param PenetrationDepth 충돌 이후 관통 깊이를 기록하는 변수
 * @param bIsStatic 해당 충돌체가 정적인지, 혹은 이동하는지 체크하는 변수
 */
class IColliderBase : public CComponent
{
private:
	int OverlapCount;
	COLLIDER_STATE ColliderState;
	Vec3 CollisionNormal;
	float PenetrationDepth;
	bool bIsStatic;

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
	// Collider State
	COLLIDER_STATE GetColliderState() const { return ColliderState; }
	bool IsActive() const { return ColliderState == ACTIVE; }
	bool IsDeactive() const { return ColliderState == DEACTIVE; }
	bool IsSemiDeactive() const { return ColliderState == SEMIDEACTIVE; }
	void SetActivate() { ColliderState = ACTIVE; }
	void SetSemiDeactivate() { ColliderState = SEMIDEACTIVE; }
	void SetDeactive() { ColliderState = DEACTIVE; }

	// Collision Data
	Vec3 GetCollisionNormal() const { return CollisionNormal; }
	float GetPenetrationDepth() const { return PenetrationDepth; }
	void SetCollisionNormal(Vec3 InNormal) { CollisionNormal = InNormal; }
	void SetPenetrationDepth(float InDepth) { PenetrationDepth = InDepth; }

	// Overlap
	bool IsOverlapped() const { return OverlapCount != 0; }
	int GetOverlapCount() const { return OverlapCount; }
	void IncreaseOverlapCount() { ++OverlapCount; }
	void DecreaseOverlapCount() { --OverlapCount; }

	// Static & Dynamic
	// 정적, 동적 충돌체를 설정하는 과정에는 BVH에서 반드시 오브젝트 컨트롤이 필요하다
	bool IsStatic() const { return bIsStatic; }
	void SetStatic() { bIsStatic = true; }
	void SetDynamic() { bIsStatic = false; }

	// Special Member Function
	~IColliderBase() override;
	IColliderBase(const IColliderBase& POrigin);
};

inline IColliderBase::IColliderBase(COMPONENT_TYPE InType)
	: CComponent(InType)
	  , OverlapCount(0)
	  , ColliderState(ACTIVE)
	  , PenetrationDepth(0)
	  , bIsStatic(true)
{
}

inline IColliderBase::IColliderBase(const IColliderBase& POrigin)
	: CComponent(POrigin)
	  , OverlapCount(0)
	  , ColliderState(POrigin.ColliderState)
	  , PenetrationDepth(0)
	  , bIsStatic(POrigin.bIsStatic)
{
}

inline IColliderBase::~IColliderBase() = default;
