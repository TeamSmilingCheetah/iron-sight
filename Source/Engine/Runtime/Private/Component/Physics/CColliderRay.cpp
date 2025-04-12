#include "pch.h"
#include "Runtime/Public/Component/Physics/CColliderRay.h"

#include "Runtime/Public/Component/Script/CScript.h"
#include "Runtime/Public/Component/Transform/CTransform.h"
#include "Runtime/Public/Component/Physics/CCollider3D.h"
#include "Runtime/Public/Component/Rendering/CLandScape.h"

CColliderRay::CColliderRay()
	:CComponent(COMPONENT_TYPE::COLLIDERRAY)
	, m_Offset(Vec3(0.f))
	, m_RayLength(1000.f)
	, m_OverlapCount(0)
	, m_RayTargetAll(false)
	, m_RayTargetLength(100000.f)
	, m_State(ACTIVE)
{
	m_RayPosDir.vStart = Vec3(0.f, 0.f, 0.f);
	m_RayPosDir.vDir = Vec3(1.f, 0.f, 0.f);
	m_RayColInfo.RayObject = this;
}

CColliderRay::CColliderRay(const CColliderRay& _Origin)
	: CComponent(_Origin)
	, m_Offset(_Origin.m_Offset)
	, m_RayLength(_Origin.m_RayLength)
	, m_OverlapCount(0)
	, m_RayTargetAll(_Origin.m_RayTargetAll)
	, m_State(_Origin.m_State)
{
	m_RayPosDir.vStart = _Origin.m_RayPosDir.vStart;
	m_RayPosDir.vDir = _Origin.m_RayPosDir.vDir;
	m_RayColInfo.RayObject = this;
}

CColliderRay::~CColliderRay()
{
}

bool CColliderRay::UpdateRayColInfo(CGameObject* _HitObject, float _Distance)
{
	// 기존 거리보다 가까운 거리에 있는 물체만 저장
	if (_Distance < m_RayColInfo.Length)
	{
		m_RayColInfo.HitObject = _HitObject;
		m_RayColInfo.Length = _Distance;
		m_RayTargetLength = m_RayColInfo.Length;
		return true;
	}

	return false;
}

void CColliderRay::ClearRayColInfo()
{
	m_RayColInfo.PrevObject = m_RayColInfo.HitObject;
	m_RayColInfo.HitObject = nullptr;
	m_RayColInfo.Length = 100000.f;
}

void CColliderRay::Activate()
{
	m_State = ACTIVE;
}

void CColliderRay::Deactivate()
{
	if (m_State != SEMIDEACTIVE)
		m_State = SEMIDEACTIVE;
}

void CColliderRay::FinalTick()
{
	if (DEACTIVE == m_State)
		return;
	else if (SEMIDEACTIVE == m_State)
		m_State = DEACTIVE;

	// 크기, 이동 행렬
	Matrix matTrans = XMMatrixTranslation(m_Offset.x, m_Offset.y, m_Offset.z);

	Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
	Matrix matScaleInv = XMMatrixInverse(nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));

	m_matColliderWorld = matTrans * matScaleInv * GetOwner()->Transform()->GetWorldMat();


	// 기본 레이 방향
	Vec3 rayDir = m_RayPosDir.vDir;
	rayDir.Normalize();

	// 레이 시작점 계산
	m_RayFinalPos = m_matColliderWorld.Translation();

	// 레이 방향 계산
	m_RayFinalDir = XMVector3TransformNormal(Vec4(rayDir.x, rayDir.y, rayDir.z, 0.f), m_matColliderWorld);
	m_RayFinalDir.Normalize();

	// 레이 끝점 계산
	Vec3 vEndPos = m_RayFinalPos + (m_RayFinalDir * m_RayLength);

	// 디버깅용 길이 끝점
	Vec3 vEndEdbugPos = m_RayFinalPos + (m_RayFinalDir * m_RayTargetLength);

	m_RayTargetLength = 100000.f;

	// 디버그 랜더링
	if (m_OverlapCount)
	{
		DrawDebugLine(Vec4(1.0f, 0.0f, 1.0f, 1.0f), m_RayFinalPos, vEndEdbugPos, false, 0.f);
	}
	else
	{
		DrawDebugLine(Vec4(0.0f, 0.0f, 1.0f, 1.0f), m_RayFinalPos, vEndPos, false, 0.f);
	}

	// 충돌 기록 초기화
	ClearRayColInfo();
}

void CColliderRay::BeginOverlap(CCollider3D* _Other)
{
	++m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(this, _Other->GetOwner(), _Other);
	}
}

void CColliderRay::Overlap(CCollider3D* _Other)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->Overlap(this, _Other->GetOwner(), _Other);
	}
}

void CColliderRay::EndOverlap(CCollider3D* _Other)
{
	--m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->EndOverlap(this, _Other->GetOwner(), _Other);
	}
}

void CColliderRay::BeginOverlap(CLandScape* _Other)
{
	++m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(this, _Other->GetOwner(), _Other);
	}
}

void CColliderRay::Overlap(CLandScape* _Other)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->Overlap(this, _Other->GetOwner(), _Other);
	}
}

void CColliderRay::EndOverlap(CLandScape* _Other)
{
	--m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(this, _Other->GetOwner(), _Other);
	}
}

void CColliderRay::SaveComponent(FILE* _File)
{
	fwrite(&m_Offset, sizeof(Vec3), 1, _File);
	fwrite(&m_RayPosDir, sizeof(tRay), 1, _File);
	fwrite(&m_RayLength, sizeof(float), 1, _File);
	fwrite(&m_RayTargetAll, sizeof(bool), 1, _File);
}

void CColliderRay::LoadComponent(FILE* _File)
{
	fread(&m_Offset, sizeof(Vec3), 1, _File);
	fread(&m_RayPosDir, sizeof(tRay), 1, _File);
	fread(&m_RayLength, sizeof(float), 1, _File);
	fread(&m_RayTargetAll, sizeof(bool), 1, _File);
}
