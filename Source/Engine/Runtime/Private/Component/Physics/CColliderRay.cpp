#include "pch.h"
#include "Runtime/Public/Component/Physics/CColliderRay.h"

#include "Runtime/Public/Component/Script/CScript.h"
#include "Runtime/Public/Component/Transform/CTransform.h"
#include "Runtime/Public/Component/Physics/CCollider3D.h"

CColliderRay::CColliderRay()
	:CComponent(COMPONENT_TYPE::COLLIDERRAY)
    , m_Offset(Vec3(0.f))
    , m_RayLength(1000.f)
	, m_OverlapCount(0)
    , m_RayTargetOne(true)
{
    m_RayPosDir.vStart = Vec3(0.f, 0.f, 0.f);
    m_RayPosDir.vDir = Vec3(1.f, 0.f, 0.f);
}

CColliderRay::CColliderRay(const CColliderRay& _Origin)
	: CComponent(_Origin)
	, m_Offset(_Origin.m_Offset)
    , m_RayLength(_Origin.m_RayLength)
	, m_OverlapCount(0)
    , m_RayTargetOne(_Origin.m_RayTargetOne)
{
    m_RayPosDir.vStart = _Origin.m_RayPosDir.vStart;
    m_RayPosDir.vDir = _Origin.m_RayPosDir.vDir;
}

CColliderRay::~CColliderRay()
{
}

void CColliderRay::FinalTick()
{
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

    // 디버그 랜더링
	if (m_OverlapCount)
	{
		DrawDebugLine(Vec4(1.0f, 0.0f, 1.0f, 1.0f), m_RayFinalPos, vEndPos, false, 0.f);
	}
	else
	{
		DrawDebugLine(Vec4(0.0f, 0.0f, 1.0f, 1.0f), m_RayFinalPos, vEndPos, false, 0.f);
	}
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

void CColliderRay::SaveComponent(FILE* _File)
{
}

void CColliderRay::LoadComponent(FILE* _File)
{
}
