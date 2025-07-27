#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/Collider3D.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"
#include "Runtime/Public/Component/Physics/CMeshCollider.h"

CColliderRay::CColliderRay()
	: CComponent(COMPONENT_TYPE::COLLIDERRAY)
	, MOffset(Vec3(0.f))
	, MLength(1000.f)
	, MTargetLength(100000.f)
	, MOverlapCount(0)
	, MColliderState(ACTIVE)
	, MIndependentDirection(false)
	, MRayTargetAll(false)
	, MTriggerTarget(true)
{
	MRayPosDir.vStart = Vec3(0.f, 0.f, 0.f);
	MRayPosDir.vDir = Vec3(1.f, 0.f, 0.f);
	MRayColliderInfo.RayObject = this;
}

CColliderRay::CColliderRay(const CColliderRay& POrigin)
	: CComponent(POrigin)
	, MOffset(POrigin.MOffset)
	, MLength(POrigin.MLength)
	, MTargetLength(POrigin.MTargetLength)
	, MOverlapCount(0)
	, MColliderState(POrigin.MColliderState)
	, MIndependentDirection(POrigin.MIndependentDirection)
	, MRayTargetAll(POrigin.MRayTargetAll)
	, MTriggerTarget(POrigin.MTriggerTarget)
{
	MRayPosDir.vStart = POrigin.MRayPosDir.vStart;
	MRayPosDir.vDir = POrigin.MRayPosDir.vDir;
	MRayColliderInfo.RayObject = this;
}

CColliderRay::~CColliderRay() = default;

bool CColliderRay::UpdateRayColInfo(CGameObject* PHitObject, float PDistance)
{
	// 기존 거리보다 가까운 거리에 있는 물체만 저장
	if (PDistance < MRayColliderInfo.Length)
	{
		MRayColliderInfo.HitObject = PHitObject;
		MRayColliderInfo.Length = PDistance;
		MTargetLength = MRayColliderInfo.Length;
		return true;
	}

	return false;
}

void CColliderRay::ClearRayColInfo()
{
	MRayColliderInfo.PrevObject = MRayColliderInfo.HitObject;
	MRayColliderInfo.HitObject = nullptr;
	MRayColliderInfo.Length = 100000.f;
}

void CColliderRay::FinalTick()
{
	if (MColliderState == DEACTIVE)
	{
		return;
	}

	if (MColliderState == SEMIDEACTIVE)
	{
		MColliderState = DEACTIVE;
	}

	// 크기, 이동 행렬
	Matrix matTrans = XMMatrixTranslation(MOffset.x, MOffset.y, MOffset.z);

	Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
	Matrix matScaleInv = XMMatrixInverse(nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));

	MWorldMatrix = matTrans * matScaleInv * GetOwner()->Transform()->GetWorldMat();

	// 기본 레이 방향
	Vec3 rayDir = MRayPosDir.vDir;
	rayDir.Normalize();

	// 레이 시작점 계산
	MFinalPosition = MWorldMatrix.Translation();

	// 독립적인지 아닌지에 따라 계산 구분
	if (MIndependentDirection)
	{
		MFinalDirection = rayDir;
	}
	else
	{
		MFinalDirection = XMVector3TransformNormal(Vec4(rayDir.x, rayDir.y, rayDir.z, 0.f), MWorldMatrix);
	}
	MFinalDirection.Normalize();

	// 레이 끝점 계산
	Vec3 vEndPos = MFinalPosition + (MFinalDirection * MLength);

	// 디버깅용 길이 끝점
	Vec3 vEndEdbugPos = MFinalPosition + (MFinalDirection * MTargetLength);

	// 히트된 오브젝트가 없을 때만 초기화
	if (!MRayColliderInfo.HitObject)
		MTargetLength = 100000.f;

	// 디버그 랜더링
	if (MOverlapCount)
	{
		DrawDebugLine(Vec4(1.0f, 0.0f, 1.0f, 1.0f), MFinalPosition, vEndEdbugPos, false, 0.f);
	}
	else
	{
		DrawDebugLine(Vec4(0.0f, 0.0f, 1.0f, 1.0f), MFinalPosition, vEndPos, false, 0.f);
	}

	// 충돌 기록 초기화
	ClearRayColInfo();
}

void CColliderRay::SaveComponent(FILE* PFile)
{
	(void)fwrite(&MOffset, sizeof(Vec3), 1, PFile);
	(void)fwrite(&MRayPosDir, sizeof(tRay), 1, PFile);
	(void)fwrite(&MLength, sizeof(float), 1, PFile);
	(void)fwrite(&MRayTargetAll, sizeof(bool), 1, PFile);

	// 추가 필요 저장 데이터
	(void)fwrite(&MIndependentDirection, sizeof(bool), 1, PFile);
	(void)fwrite(&MRayTargetAll, sizeof(bool), 1, PFile);
	(void)fwrite(&MTriggerTarget, sizeof(bool), 1, PFile);
}

void CColliderRay::LoadComponent(FILE* PFile)
{
	(void)fread(&MOffset, sizeof(Vec3), 1, PFile);
	(void)fread(&MRayPosDir, sizeof(tRay), 1, PFile);
	(void)fread(&MLength, sizeof(float), 1, PFile);
	(void)fread(&MRayTargetAll, sizeof(bool), 1, PFile);

	// 추가 필요 로드 데이터
	(void)fread(&MIndependentDirection, sizeof(bool), 1, PFile);
	(void)fread(&MRayTargetAll, sizeof(bool), 1, PFile);
	(void)fread(&MTriggerTarget, sizeof(bool), 1, PFile);
}
