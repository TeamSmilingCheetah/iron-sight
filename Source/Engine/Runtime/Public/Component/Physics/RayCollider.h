#pragma once
#include "ColliderBase.h"

/**
 * @brief Raycast 처리를 위한 Ray 충돌체 클래스
 *
 * @param Offset Local 좌표계에서의 Ray 시작점
 * @param Direction Local 좌표계 기준 Ray 방향, IndependentDirection이면 해당 방향을 사용함
 * @param FinalPosition World 좌표계 기준 Ray 시작점
 * @param FinalDirection World 좌표계 기준 Ray 방향
 * @param WorldMatrix 크기, 회전, 이동
 * @param Length Ray의 길이
 * @param bIndependentDirection 독립적인 방향을 가지는지 여부
 * @param bRayTargetAll Target이 단일인지, 관통을 통한 다수의 Target을 가지는지 여부
 * @param bTriggerTarget 트리거용 충돌체를 감지할지 여부
 * @param HitDistance Ray와 충돌하는 가장 가까운 타겟과의 거리
 * @param HitNormal Ray와 충돌하는 가장 가까운 타겟의 충돌 Normal
 */
class FRayCollider :
	public IColliderBase
{
private:
	Vec3 Offset;
	Vec3 Direction;
	Vec3 FinalPosition;
	Vec3 FinalDirection;

	Matrix WorldMatrix;
	float Length;

	bool bIndependentDirection;
	bool bRayTargetAll;
	bool bTriggerTarget;

	vector<float> HitDistances;
	vector<Vec3> HitNormals;

public:
	void FinalTick() override;
	void SaveComponent(FILE* InFile) override;
	void LoadComponent(FILE* InFile) override;
	const AABB GetAABB() const override;

	// bool UpdateRayColInfo(IColliderBase* InHitCollider, float InDistance);
	// void ClearRayColInfo();

	// Getter & Setter
	EColliderType GetColliderType() const override { return EColliderType::RayCollider; }
	Vec3 GetOffset() const { return Offset; }
	Vec3 GetDirection() const { return Direction; }
	float GetLength() const { return Length; }
	const Matrix& GetColliderWorldMat() const { return WorldMatrix; }

	Vec3 GetFinalPosition() const { return FinalPosition; }
	Vec3 GetFinalDirection() const { return FinalDirection; }

	bool IsIndependentDir() const { return bIndependentDirection; }
	bool IsTargetAllMode() const { return bRayTargetAll; }
	bool IsTriggerTarget() const { return bTriggerTarget; }

	void SetOffset(Vec3 InOffset) { Offset = InOffset; }
	void SetDirection(Vec3 InDirection) { Direction = InDirection; }
	void SetFinalDirection(Vec3 InDirection) { FinalDirection = InDirection; }
	void SetLength(float InLength) { Length = InLength; }
	void SetRayTargetMode(bool InIsTargetAll) { bRayTargetAll = InIsTargetAll; }
	void SetIndependentDir(bool InIsIndepenedent) { bIndependentDirection = InIsIndepenedent; }
	void SetTriggerTarget(bool InIsTrigger) { bTriggerTarget = InIsTrigger; }

	void SetHitDistance(float InHitDistance) { HitDistances.push_back(InHitDistance); }
	void SetHitNormal(Vec3 InHitNormal) { HitNormals.push_back(InHitNormal); }

	// Special Member Function
	FRayCollider();
	FRayCollider(const FRayCollider& POrigin);
	~FRayCollider() override;
	CLONE(FRayCollider);
};
