#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"

#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

// Constant Structure
constexpr static Vec3 CubeArr[8] = {
	Vec3(-0.5f, 0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f),
	Vec3(0.5f, -0.5f, 0.5f), Vec3(-0.5f, -0.5f, 0.5f),
	Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f),
	Vec3(0.5f, -0.5f, -0.5f), Vec3(-0.5f, -0.5f, -0.5f)
};

FBoxCollider::FBoxCollider()
	: IColliderBase(COMPONENT_TYPE::BOX_COLLIDER)
	  , Offset(Vec3(0.f))
	  , Scale(Vec3(0.f))
	  , RotY(0)
	  , Status(0)
	  , bIndependentScale(false)
{
}

FBoxCollider::~FBoxCollider() = default;

FBoxCollider::FBoxCollider(const FBoxCollider& POrigin)
	: IColliderBase(POrigin)
	  , Offset(POrigin.Offset)
	  , Scale(POrigin.Scale)
	  , RotY(0), Status(POrigin.Status)
	  , bIndependentScale(POrigin.bIndependentScale)
	  , FinalPos(POrigin.FinalPos)
{
}

void FBoxCollider::FinalTick()
{
	if (IsDeactive())
	{
		return;
	}
	if (IsSemiDeactive())
	{
		SetDeactive();
	}

	Matrix matScale = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	Matrix matTrans = XMMatrixTranslation(Offset.x, Offset.y, Offset.z);
	Matrix matRot = XMMatrixRotationY(RotY);

	Vec3 ownerPos = GetOwner()->Transform()->GetWorldPos();
	Vec3 ownerScale = GetOwner()->Transform()->GetWorldScale();

	Matrix ownerScaleMat = XMMatrixScaling(ownerScale.x, ownerScale.y, ownerScale.z);
	Matrix ownerTransMat = XMMatrixTranslation(ownerPos.x, ownerPos.y, ownerPos.z);

	Matrix ownerMatNoRot = ownerScaleMat * ownerTransMat;

	if (Status & INDEPENDENT_ROT)
	{
		if (bIndependentScale)
		{
			Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
			Matrix matScaleInv = XMMatrixInverse(
				nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));
			MatrixColliderWorld = matScale * matRot * matTrans * matScaleInv * ownerMatNoRot;
		}
		else
		{
			MatrixColliderWorld = matScale * matRot * matTrans * ownerMatNoRot;
		}
	}
	else
	{
		if (bIndependentScale)
		{
			Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
			Matrix matScaleInv = XMMatrixInverse(
				nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));
			MatrixColliderWorld = matScale * matRot * matTrans * matScaleInv * GetOwner()->Transform()->GetWorldMat();
		}
		else
		{
			MatrixColliderWorld = matScale * matRot * matTrans * GetOwner()->Transform()->GetWorldMat();
		}
	}

	if (IsOverlapped())
	{
		DrawDebugCube(Vec4(1.f, 0.f, 0.f, 1.f), MatrixColliderWorld, false, 0.f);
	}
	else
	{
		DrawDebugCube(Vec4(0.f, 1.f, 0.f, 1.f), MatrixColliderWorld, false, 0.f);
	}
}

void FBoxCollider::SetTrigger(bool InTriggerStatus)
{
	if (InTriggerStatus)
	{
		Status |= TRIGGER;
	}
	else
	{
		Status &= ~TRIGGER;
	}
}

void FBoxCollider::SetIndependentRot(bool InIsIndependentRotation)
{
	if (InIsIndependentRotation)
	{
		Status |= INDEPENDENT_ROT;
	}
	else
	{
		Status &= ~INDEPENDENT_ROT;
	}
}

void FBoxCollider::SaveComponent(FILE* PFile)
{
	(void)fwrite(&Offset, sizeof(Vec3), 1, PFile);
	(void)fwrite(&Scale, sizeof(Vec3), 1, PFile);
	(void)fwrite(&FinalPos, sizeof(Vec3), 1, PFile);
	(void)fwrite(&bIndependentScale, sizeof(bool), 1, PFile);

	// 추가 필요 저장 데이터
	(void)fwrite(&Status, sizeof(int), 1, PFile);

	bool IsStaticCollider = IsStatic();
	(void)fwrite(&IsStaticCollider, sizeof(bool), 1, PFile);
}

void FBoxCollider::LoadComponent(FILE* PFile)
{
	(void)fread(&Offset, sizeof(Vec3), 1, PFile);
	(void)fread(&Scale, sizeof(Vec3), 1, PFile);
	(void)fread(&FinalPos, sizeof(Vec3), 1, PFile);
	(void)fread(&bIndependentScale, sizeof(bool), 1, PFile);

	// 추가 필요 로드 데이터
	(void)fread(&Status, sizeof(int), 1, PFile);

	bool IsStaticCollider;
	(void)fread(&IsStaticCollider, sizeof(bool), 1, PFile);

	if (!IsStaticCollider)
	{
		SetDynamic();
	}
	else
	{
		SetStatic();
	}
}

/**
 * @brief 3D Collider 기준 AABB를 산출하는 함수
 *
 * @return { AABB Min, AABB Max }
 */
const AABB FBoxCollider::GetAABB() const
{
	Matrix WorldMatrix = GetColliderWorldMat();
	Vec3 Vtxs[8];
	for (int i = 0; i < 8; ++i)
	{
		Vtxs[i] = XMVector3TransformCoord(CubeArr[i], WorldMatrix);
	}

	Vec3 BoxMin = Vtxs[0];
	Vec3 BoxMax = Vtxs[0];

	for (int i = 1; i < 8; ++i)
	{
		BoxMin.x = min(BoxMin.x, Vtxs[i].x);
		BoxMin.y = min(BoxMin.y, Vtxs[i].y);
		BoxMin.z = min(BoxMin.z, Vtxs[i].z);
		BoxMax.x = max(BoxMax.x, Vtxs[i].x);
		BoxMax.y = max(BoxMax.y, Vtxs[i].y);
		BoxMax.z = max(BoxMax.z, Vtxs[i].z);
	}

	// Make Min Thickness
	constexpr float MIN_THICKNESS = 1.0f;
	if (fabs(BoxMax.x - BoxMin.x) < MIN_THICKNESS)
	{
		float Center = (BoxMax.x + BoxMin.x) * 0.5f;
		BoxMin.x = Center - MIN_THICKNESS * 0.5f;
		BoxMax.x = Center + MIN_THICKNESS * 0.5f;
	}
	if (fabs(BoxMax.y - BoxMin.y) < MIN_THICKNESS)
	{
		float Center = (BoxMax.y + BoxMin.y) * 0.5f;
		BoxMin.y = Center - MIN_THICKNESS * 0.5f;
		BoxMax.y = Center + MIN_THICKNESS * 0.5f;
	}
	if (fabs(BoxMax.z - BoxMin.z) < MIN_THICKNESS)
	{
		float Center = (BoxMax.z + BoxMin.z) * 0.5f;
		BoxMin.z = Center - MIN_THICKNESS * 0.5f;
		BoxMax.z = Center + MIN_THICKNESS * 0.5f;
	}

	return AABB(BoxMin, BoxMax);
}
