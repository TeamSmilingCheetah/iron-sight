#pragma once
#include "ColliderBase.h"

/**
 * @brief 오브젝트에 충돌체 속성을 추가하는 컴포넌트 클래스
 *
 * @param Offset 충돌체의 로컬 위치 오프셋
 * @param Scale 충돌체의 크기 스케일
 * @param RotY 충돌체의 Y축 회전값
 * @param FinalPos 충돌체의 최종 월드 위치
 * @param HitNormal 충돌 시 충돌면의 법선 벡터
 * @param HitPoint 충돌 시 충돌 지점의 위치
 * @param MatrixColliderWorld 충돌체의 월드 변환 행렬 (크기, 회전, 이동 정보 포함)
 * @param OverlapCount 현재 겹침 상태인 다른 충돌체의 수
 * @param Status 충돌체의 상태 플래그 (트리거, 독립 회전 등)
 * @param IndependentScale 오브젝트의 스케일과 독립적으로 동작하는지 여부
 */
class FBoxCollider :
	public IColliderBase
{
private:
	Vec3 Offset;
	Vec3 Scale;
	float RotY;
	Vec3 HitPoint;
	Vec3 ClosestPoint;
	Matrix MatrixColliderWorld;

	int Status;
	bool bIndependentScale;

	// TODO(KHJ): 안 쓰는 변수 제거, 현재는 파일 저장되는 변수라 제거 보류
	// [[deprecated]]
	Vec3 FinalPos;

public:
	void FinalTick() override;
	const AABB GetAABB() const override;

	void SaveComponent(FILE* PFile) override;
	void LoadComponent(FILE* PFile) override;

	// Getter & Setter
	EColliderType GetColliderType() const override { return EColliderType::BoxCollider; }
	Vec3 GetOffset() const { return Offset; }
	Vec3 GetScale() const { return Scale; }
	float GetRotY() const { return RotY; }
	Vec3 GetHitPoint() const { return HitPoint; }
	Vec3 GetClosestPoint() const { return ClosestPoint; }
	const Matrix& GetColliderWorldMat() const { return MatrixColliderWorld; }
	bool IsTrigger() const { return (Status & TRIGGER); }
	bool IsIndependentRot() const { return (Status & INDEPENDENT_ROT); }
	bool IsIndependentScale() const { return bIndependentScale; }

	void SetOffset(Vec3 InOffset) { Offset = InOffset; }
	void SetScale(Vec3 InScale) { Scale = InScale; }
	void SetScaleMultiplier(float InSize) { Scale *= InSize; }
	void SetRotY(float InRot) { RotY = InRot; }
	void SetHitPoint(Vec3 InHitPoint) { HitPoint = InHitPoint; }
	void SetIndependentScale(bool InScale) { bIndependentScale = InScale; }
	void SetClosestPoint(Vec3 InClosestPoint) { ClosestPoint = InClosestPoint; }
	void SetTrigger(bool InTriggerStatus);
	void SetIndependentRot(bool InIsIndependentRotation);

	// Special Member Function
	CLONE(FBoxCollider);
	FBoxCollider();
	FBoxCollider(const FBoxCollider& POrigin);
	~FBoxCollider() override;
};
