#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/SphereCollider.h"

FSphereCollider::FSphereCollider()
	: IColliderBase(COMPONENT_TYPE::SPHERE_COLLIDER)
	  , Scale(0.f)
{
}

FSphereCollider::~FSphereCollider() = default;

FSphereCollider::FSphereCollider(const FSphereCollider& InOrigin)
	: IColliderBase(COMPONENT_TYPE::SPHERE_COLLIDER)
	  , Scale(InOrigin.Scale)
{
}

void FSphereCollider::Init()
{
}

void FSphereCollider::FinalTick()
{
}

const AABB FSphereCollider::GetAABB() const
{
	return AABB();
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

	(void)fwrite(&Offset, sizeof(Vec3), 1, InFile);
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
