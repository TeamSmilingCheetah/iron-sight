#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/SphereCollider.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

FSphereCollider::FSphereCollider()
	: IColliderBase(COMPONENT_TYPE::SPHERE_COLLIDER)
	  , Offset(Vec3(0, 0, 0))
	  , WorldOffset(Vec3(0, 0, 0))
	  , Scale(1.f)
	  , WorldScale(1.f)
	  , bHasIndependentScale(false)
{
}

FSphereCollider::~FSphereCollider() = default;

FSphereCollider::FSphereCollider(const FSphereCollider& InOrigin) = default;

void FSphereCollider::Init()
{
}

void FSphereCollider::FinalTick()
{
	if (IsDeactive())
	{
		return;
	}
	if (IsSemiDeactive())
	{
		SetDeactive();
	}

	// Get World Offset
	Matrix OwnerWorldMatrix = GetOwner()->Transform()->GetWorldMat();
	WorldOffset = XMVector3Transform(XMLoadFloat3(&Offset), OwnerWorldMatrix);

	// Get World Scale
	if (bHasIndependentScale)
	{
		WorldScale = Scale;
	}
	else
	{
		Vec3 OwnerScale = GetOwner()->Transform()->GetWorldScale();
		float MaxScale = max(OwnerScale.x, max(OwnerScale.y, OwnerScale.z));

		WorldScale = Scale * MaxScale;
	}

    // Debug Render
    if (IsOverlapped())
    {
        DrawDebugSphere(Vec4(1.f, 0.f, 0.f, 1.f), WorldOffset, WorldScale, false, 0.f);
    }
    else
    {
        DrawDebugSphere(Vec4(0.f, 1.f, 0.f, 1.f), WorldOffset, WorldScale, false, 0.f);
    }
}

const AABB FSphereCollider::GetAABB() const
{
    if (WorldScale <= 0.f)
    {
        return AABB();
    }

    const Vec3 Radius(WorldScale);
    const Vec3 MinBound = WorldOffset - Radius;
    const Vec3 MaxBound = WorldOffset + Radius;

    return AABB(MinBound, MaxBound);
}

/*****************/
/** Save & Load **/
/*****************/

void FSphereCollider::SaveComponent(FILE* InFile)
{
	bool IsStaticCollider = IsStatic();

	(void)fwrite(&Offset, sizeof(Vec3), 1, InFile);
	(void)fwrite(&Scale, sizeof(float), 1, InFile);
	(void)fwrite(&IsStaticCollider, sizeof(bool), 1, InFile);
}

void FSphereCollider::LoadComponent(FILE* InFile)
{
	bool IsStaticCollider;

	(void)fread(&Offset, sizeof(Vec3), 1, InFile);
	(void)fread(&Scale, sizeof(float), 1, InFile);
	(void)fread(&IsStaticCollider, sizeof(bool), 1, InFile);

	if (!IsStaticCollider)
	{
		SetDynamic();
	}
	else
	{
		SetStatic();
	}
}
