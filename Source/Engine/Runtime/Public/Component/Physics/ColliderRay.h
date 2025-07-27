#pragma once
#include "ColliderBase.h"

/**
 * @brief Raycast 처리를 위한 Ray 충돌체 클래스
 *
 * @param MOffset
 * @param MRayPosDir
 * @param MWorldMatrix 크기, 회전, 이동
 * @param MFinalPosition 최종 레이의 위치
 * @param MFinalDirection 최종 레이의 방향
 * @param MLength Ray 길이
 * @param MTargetLength 디버그용 임시 길이
 * @param MOverlapCount
 * @param MColliderState 충돌체 상태
 * @param MRayColliderInfo 단일 타겟 용 검사 구조체
 * @param MIndependentDirection 독립적인 방향
 * @param MRayTargetAll 레이가 발견 가능한 타겟 판정
 * @param MTriggerTarget 트리거용 충돌체를 감지할지 판정
 */
class FColliderRay :
	public IColliderBase
{
private:
	Vec3 Offset;
	tRay RayPosDir;
	Matrix WorldMatrix;

	Vec3 FinalPosition;
	Vec3 FinalDirection;

	float Length;
	float TargetLength;

	int OverlapCount;

	COLLIDER_STATE ColliderState;
	FRayColliderInfo RayColliderInfo;

	bool bIndependentDirection;
	bool bRayTargetAll;
	bool bTriggerTarget;

public:
	void FinalTick() override;
	void SaveComponent(FILE* InFile) override;
	void LoadComponent(FILE* InFile) override;
	const AABB GetAABB() const override;

	bool UpdateRayColInfo(CGameObject* InHitObject, float InDistance);
	void ClearRayColInfo();

	// Getter & Setter
	tRay GetRay() const { return RayPosDir; }
	Vec3 GetRayPos() const { return RayPosDir.vStart; }
	Vec3 GetRayDir() const { return RayPosDir.vDir; }
	Vec3 GetOffset() const { return Offset; }
	float GetRayLength() const { return Length; }
	const Matrix& GetColliderWorldMat() const { return WorldMatrix; }

	Vec3 GetRayFinalPos() const { return FinalPosition; }
	Vec3 GetRayFinalDir() const { return FinalDirection; }
	bool IsIndependentDir() const { return bIndependentDirection; }

	FRayColliderInfo& GetTargetInfoRef() { return RayColliderInfo; }

	bool IsTargetAllMode() const { return bRayTargetAll; }
	bool IsTriggerTarget() const { return bTriggerTarget; }

	void SetRayLength(float PLength) { Length = PLength; }
	void SetRayTargetMode(bool PTargetAll) { bRayTargetAll = PTargetAll; }
	void SetRayTargetLength(float PTargetLength) { TargetLength = PTargetLength; }
	void SetIndependentDir(bool PVal) { bIndependentDirection = PVal; }
	void SetTriggerTarget(bool PVal) { bTriggerTarget = PVal; }

	void SetOffset(Vec3 POffset) { Offset = POffset; }
	void SetRayPos(Vec3 PPos) { RayPosDir.vStart = PPos; }

	void SetRayDir(Vec3 PDir)
	{
		RayPosDir.vDir = PDir;
		RayPosDir.vDir.Normalize();
	}

	// Special Member Function
	FColliderRay();
	FColliderRay(const FColliderRay& POrigin);
	~FColliderRay() override;
	CLONE(FColliderRay);
};
