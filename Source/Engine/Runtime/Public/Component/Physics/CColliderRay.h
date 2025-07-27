#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"

/**
 * @brief
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
class CColliderRay :
	public CComponent
{
	friend class CollisionManager;

private:
	Vec3 MOffset;
	tRay MRayPosDir;
	Matrix MWorldMatrix;

	Vec3 MFinalPosition;
	Vec3 MFinalDirection;

	float MLength;
	float MTargetLength;

	int MOverlapCount;

	COLLIDER_STATE MColliderState;
	RayColliderInfo MRayColliderInfo;

	bool MIndependentDirection;
	bool MRayTargetAll;
	bool MTriggerTarget;

public:
	void FinalTick() override;
	void SaveComponent(FILE* PFile) override;
	void LoadComponent(FILE* PFile) override;

	bool UpdateRayColInfo(CGameObject* PHitObject, float PDistance);
	void ClearRayColInfo();

	template <typename T>
	void BeginOverlap(T* POther);
	template <typename T>
	void Overlap(T* POther);
	template <typename T>
	void EndOverlap(T* POther);

	// Getter & Setter
	void SetActivate() { MColliderState = ACTIVE; }
	void SetSemiDeactivate() { MColliderState = SEMIDEACTIVE; }

	tRay GetRay() const { return MRayPosDir; }
	Vec3 GetRayPos() const { return MRayPosDir.vStart; }
	Vec3 GetRayDir() const { return MRayPosDir.vDir; }
	Vec3 GetOffset() const { return MOffset; }
	float GetRayLength() const { return MLength; }
	const Matrix& GetColliderWorldMat() const { return MWorldMatrix; }

	Vec3 GetRayFinalPos() const { return MFinalPosition; }
	Vec3 GetRayFinalDir() const { return MFinalDirection; }
	bool IsIndependentDir() const { return MIndependentDirection; }

	RayColliderInfo& GetTargetInfoRef() { return MRayColliderInfo; }

	bool IsTargetAllMode() const { return MRayTargetAll; }
	bool IsTriggerTarget() const { return MTriggerTarget; }

	COLLIDER_STATE GetState() const { return MColliderState; }
	bool IsActive() const { return MColliderState == ACTIVE; }

	void SetRayLength(float PLength) { MLength = PLength; }
	void SetRayTargetMode(bool PTargetAll) { MRayTargetAll = PTargetAll; }
	void SetRayTargetLength(float PTargetLength) { MTargetLength = PTargetLength; }
	void SetIndependentDir(bool PVal) { MIndependentDirection = PVal; }
	void SetTriggerTarget(bool PVal) { MTriggerTarget = PVal; }

	void SetOffset(Vec3 POffset) { MOffset = POffset; }
	void SetRayPos(Vec3 PPos) { MRayPosDir.vStart = PPos; }

	void SetRayDir(Vec3 PDir)
	{
		MRayPosDir.vDir = PDir;
		MRayPosDir.vDir.Normalize();
	}

	// Special Member Function
	CLONE(CColliderRay);
	CColliderRay();
	CColliderRay(const CColliderRay& POrigin);
	~CColliderRay() override;
};

template <typename T>
void CColliderRay::BeginOverlap(T* POther)
{
	++MOverlapCount;

	const vector<CScript*>& ScriptVector = GetOwner()->GetScripts();
	for (auto* Script : ScriptVector)
	{
		Script->BeginOverlap(this, POther->GetOwner(), POther);
	}
}

template <typename T>
void CColliderRay::Overlap(T* POther)
{
	const vector<CScript*>& ScriptVector = GetOwner()->GetScripts();
	for (auto* Script : ScriptVector)
	{
		Script->Overlap(this, POther->GetOwner(), POther);
	}
}

template <typename T>
void CColliderRay::EndOverlap(T* POther)
{
	--MOverlapCount;

	const vector<CScript*>& ScriptVector = GetOwner()->GetScripts();
	for (auto* Script : ScriptVector)
	{
		Script->EndOverlap(this, POther->GetOwner(), POther);
	}

	CGameObject* OtherObj = POther->GetOwner();
	// 충돌된 오브젝트가 이제 삭제 처리될 오브젝트일 시 prev에 들어가지 않게 비운다.
	if (OtherObj == MRayColliderInfo.HitObject && OtherObj->IsDead())
	{
		MRayColliderInfo.HitObject = nullptr;
	}
}
