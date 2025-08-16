#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/RayCollider.h"

#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

using std::ranges::min_element;

constexpr Vec3 DefaultNormal = Vec3(0, 1, 0);

FRayCollider::FRayCollider()
	: IColliderBase(COMPONENT_TYPE::RAY_COLLIDER)
	, Offset(Vec3(0.f))
	, Length(1000.f)
	, bIndependentDirection(false)
	, bRayTargetAll(false)
	, bTriggerTarget(true)
{
}

FRayCollider::FRayCollider(const FRayCollider& POrigin)
	: IColliderBase(POrigin)
	, Offset(POrigin.Offset)
	, Length(POrigin.Length)
	, bIndependentDirection(POrigin.bIndependentDirection)
	, bRayTargetAll(POrigin.bRayTargetAll)
	, bTriggerTarget(POrigin.bTriggerTarget)
{
}

FRayCollider::~FRayCollider() = default;

float FRayCollider::GetClosestHitDistance() const
{
	if (HitDistances.empty())
	{
		return -1.f;
	}
	return *min_element(HitDistances);
}

Vec3 FRayCollider::GetClosestHitNormal() const
{
	if (HitDistances.empty())
	{
		return DefaultNormal;
	}

	auto MinIterator = min_element(HitDistances);
	size_t MinIndex = distance(HitDistances.begin(), MinIterator);

	if (MinIndex < HitNormals.size())
	{
		return HitNormals[MinIndex];
	}

	return DefaultNormal;
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

	// 매 틱마다 Hit 정보 초기화
	HitDistances.clear();
	HitNormals.clear();

	// 크기, 이동 행렬
	Matrix matTrans = XMMatrixTranslation(Offset.x, Offset.y, Offset.z);

	Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
	Matrix matScaleInv = XMMatrixInverse(nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));

	WorldMatrix = matTrans * matScaleInv * GetOwner()->Transform()->GetWorldMat();

	// 기본 레이 방향
	Direction = Direction.Normalized();

	// 레이 시작점 계산
	FinalPosition = WorldMatrix.Translation();

	// 독립적인지 아닌지에 따라 계산 구분
	if (bIndependentDirection)
	{
		FinalDirection = Direction;
	}
	else
	{
		FinalDirection = XMVector3TransformNormal(Vec4(Direction.x, Direction.y, Direction.z, 0.f), WorldMatrix);
	}

	FinalDirection.Normalize();

	// 레이 끝점 계산
	Vec3 EndPosition = FinalPosition + (FinalDirection * Length);

	// 디버그 랜더링
	if (IsOverlapped())
	{
		DrawDebugLine(Vec4(1.0f, 0.0f, 1.0f, 1.0f), FinalPosition, EndPosition, false, 0.f);
	}
	else
	{
		DrawDebugLine(Vec4(0.0f, 0.0f, 1.0f, 1.0f), FinalPosition, EndPosition, false, 0.f);
	}
}

void FRayCollider::SaveComponent(FILE* InFile)
{
	// TODO(KHJ): tRay 구조체 잔재 정리할 것
	Vec3 Dummy[2];

	(void)fwrite(&Offset, sizeof(Vec3), 1, InFile);
	(void)fwrite(&Dummy, sizeof(Vec3[2]), 1, InFile);
	(void)fwrite(&Length, sizeof(float), 1, InFile);
	(void)fwrite(&bRayTargetAll, sizeof(bool), 1, InFile);

	// 추가 필요 저장 데이터
	(void)fwrite(&bIndependentDirection, sizeof(bool), 1, InFile);
	(void)fwrite(&bRayTargetAll, sizeof(bool), 1, InFile);
	(void)fwrite(&bTriggerTarget, sizeof(bool), 1, InFile);
}

void FRayCollider::LoadComponent(FILE* InFile)
{
	// TODO(KHJ): tRay 구조체 잔재 정리할 것
	Vec3 Dummy[2];

	(void)fread(&Offset, sizeof(Vec3), 1, InFile);
	(void)fread(&Dummy, sizeof(Vec3[2]), 1, InFile);
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
