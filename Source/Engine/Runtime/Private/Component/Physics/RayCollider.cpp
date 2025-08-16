#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/RayCollider.h"

#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

FRayCollider::FRayCollider()
	: IColliderBase(COMPONENT_TYPE::RAY_COLLIDER)
	, Offset(Vec3(0.f))
	, Length(1000.f)
	, TargetLength(100000.f)
	, bIndependentDirection(false)
	, bRayTargetAll(false)
	, bTriggerTarget(true)
{
	RayPosDir.vStart = Vec3(0.f, 0.f, 0.f);
	RayPosDir.vDir = Vec3(1.f, 0.f, 0.f);
	RayCollisionInfo.RayObject = this;
}

FRayCollider::FRayCollider(const FRayCollider& POrigin)
	: IColliderBase(POrigin)
	, Offset(POrigin.Offset)
	, Length(POrigin.Length)
	, TargetLength(POrigin.TargetLength)
	, bIndependentDirection(POrigin.bIndependentDirection)
	, bRayTargetAll(POrigin.bRayTargetAll)
	, bTriggerTarget(POrigin.bTriggerTarget)
{
	RayPosDir.vStart = POrigin.RayPosDir.vStart;
	RayPosDir.vDir = POrigin.RayPosDir.vDir;
	RayCollisionInfo.RayObject = this;
}

FRayCollider::~FRayCollider() = default;

bool FRayCollider::UpdateRayColInfo(IColliderBase* InHitCollider, float InDistance)
{
	// 기존 거리보다 가까운 거리에 있는 물체만 저장
	if (InDistance < RayCollisionInfo.Length)
	{
		RayCollisionInfo.HitCollider = InHitCollider;
		RayCollisionInfo.Length = InDistance;
		TargetLength = RayCollisionInfo.Length;
		return true;
	}

	return false;
}

void FRayCollider::ClearRayColInfo()
{
	RayCollisionInfo.PrevCollider = RayCollisionInfo.HitCollider;
	RayCollisionInfo.HitCollider = nullptr;
	RayCollisionInfo.Length = 100000.f;
}

void FRayCollider::FinalTick()
{
	if (IsDeactive())
	{
		return;
	}

	if (IsSemiDeactive())
	{
		SetDeactive();
	}

	// 크기, 이동 행렬
	Matrix matTrans = XMMatrixTranslation(Offset.x, Offset.y, Offset.z);

	Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
	Matrix matScaleInv = XMMatrixInverse(nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));

	WorldMatrix = matTrans * matScaleInv * GetOwner()->Transform()->GetWorldMat();

	// 기본 레이 방향
	Vec3 rayDir = RayPosDir.vDir;
	rayDir.Normalize();

	// 레이 시작점 계산
	FinalPosition = WorldMatrix.Translation();

	// 독립적인지 아닌지에 따라 계산 구분
	if (bIndependentDirection)
	{
		FinalDirection = rayDir;
	}
	else
	{
		FinalDirection = XMVector3TransformNormal(Vec4(rayDir.x, rayDir.y, rayDir.z, 0.f), WorldMatrix);
	}
	FinalDirection.Normalize();

	// 레이 끝점 계산
	Vec3 vEndPos = FinalPosition + (FinalDirection * Length);

	// 디버깅용 길이 끝점
	Vec3 vEndEdbugPos = FinalPosition + (FinalDirection * TargetLength);

	// 히트된 오브젝트가 없을 때만 초기화
	if (!RayCollisionInfo.HitCollider)
	{
		TargetLength = 100000.f;
	}

	// 디버그 랜더링
	if (IsOverlapped())
	{
		DrawDebugLine(Vec4(1.0f, 0.0f, 1.0f, 1.0f), FinalPosition, vEndEdbugPos, false, 0.f);
	}
	else
	{
		DrawDebugLine(Vec4(0.0f, 0.0f, 1.0f, 1.0f), FinalPosition, vEndPos, false, 0.f);
	}

	// 충돌 기록 초기화
	ClearRayColInfo();
}

void FRayCollider::SaveComponent(FILE* InFile)
{
	(void)fwrite(&Offset, sizeof(Vec3), 1, InFile);
	(void)fwrite(&RayPosDir, sizeof(tRay), 1, InFile);
	(void)fwrite(&Length, sizeof(float), 1, InFile);
	(void)fwrite(&bRayTargetAll, sizeof(bool), 1, InFile);

	// 추가 필요 저장 데이터
	(void)fwrite(&bIndependentDirection, sizeof(bool), 1, InFile);
	(void)fwrite(&bRayTargetAll, sizeof(bool), 1, InFile);
	(void)fwrite(&bTriggerTarget, sizeof(bool), 1, InFile);
}

void FRayCollider::LoadComponent(FILE* InFile)
{
	(void)fread(&Offset, sizeof(Vec3), 1, InFile);
	(void)fread(&RayPosDir, sizeof(tRay), 1, InFile);
	(void)fread(&Length, sizeof(float), 1, InFile);
	(void)fread(&bRayTargetAll, sizeof(bool), 1, InFile);

	// 추가 필요 로드 데이터
	(void)fread(&bIndependentDirection, sizeof(bool), 1, InFile);
	(void)fread(&bRayTargetAll, sizeof(bool), 1, InFile);
	(void)fread(&bTriggerTarget, sizeof(bool), 1, InFile);
}

/**
 * @brief 순수 가상 함수로 작성된 GetAABB에 대한 구현부, 호출될 수 없도록 컨트롤
 * @return Null AABB
 */
const AABB FRayCollider::GetAABB() const
{
	LOG_ERROR_F("[Collision][Ray] {}: RayCollider Don't Have AABB", WStringToString(GetOwner()->GetName()));
	assert(!"RayCollider Don't Have AABB");
	return AABB();
}
