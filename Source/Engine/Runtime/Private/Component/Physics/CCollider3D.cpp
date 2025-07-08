#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"

#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Physics/CMeshCollider.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

CCollider3D::CCollider3D()
	: CComponent(COMPONENT_TYPE::COLLIDER3D)
	  , m_Offset(Vec3(0.f))
	  , m_Scale(Vec3(0.f))
	  , m_RotY(0)
	  , m_OverlapCount(0)
	  , m_Status(0), m_State(ACTIVE)
	  , m_IndependentScale(false)
{
}

CCollider3D::~CCollider3D() = default;

CCollider3D::CCollider3D(const CCollider3D& POrigin)
	: CComponent(POrigin)
	  , m_Offset(POrigin.m_Offset)
	  , m_Scale(POrigin.m_Scale)
	  , m_RotY(0), m_FinalPos(POrigin.m_FinalPos)
	  , m_OverlapCount(0)
	  , m_Status(POrigin.m_Status)
	  , m_State(POrigin.m_State)
	  , m_IndependentScale(POrigin.m_IndependentScale)
{
}

void CCollider3D::Activate()
{
	m_State = ACTIVE;
}

void CCollider3D::Deactivate()
{
	if (m_State != SEMIDEACTIVE)
		m_State = SEMIDEACTIVE;
}

void CCollider3D::FinalTick()
{
	if (DEACTIVE == m_State)
		return;
	else if (SEMIDEACTIVE == m_State)
		m_State = DEACTIVE;

	Matrix matScale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	Matrix matTrans = XMMatrixTranslation(m_Offset.x, m_Offset.y, m_Offset.z);
	Matrix matRot = XMMatrixRotationY(m_RotY);

	Vec3 ownerPos = GetOwner()->Transform()->GetWorldPos();
	Vec3 ownerScale = GetOwner()->Transform()->GetWorldScale();

	Matrix ownerScaleMat = XMMatrixScaling(ownerScale.x, ownerScale.y, ownerScale.z);
	Matrix ownerTransMat = XMMatrixTranslation(ownerPos.x, ownerPos.y, ownerPos.z);

	Matrix ownerMatNoRot = ownerScaleMat * ownerTransMat;

	if (m_Status & INDEPENDENT_ROT)
	{
		if (m_IndependentScale)
		{
			Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
			Matrix matScaleInv = XMMatrixInverse(
				nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));
			m_matColliderWorld = matScale * matRot * matTrans * matScaleInv * ownerMatNoRot;
		}
		else
		{
			m_matColliderWorld = matScale * matRot * matTrans * ownerMatNoRot;
		}
	}
	else
	{
		if (m_IndependentScale)
		{
			Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
			Matrix matScaleInv = XMMatrixInverse(
				nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));
			m_matColliderWorld = matScale * matRot * matTrans * matScaleInv * GetOwner()->Transform()->GetWorldMat();
		}
		else
		{
			m_matColliderWorld = matScale * matRot * matTrans * GetOwner()->Transform()->GetWorldMat();
		}
	}


	if (m_OverlapCount)
	{
		DrawDebugCube(Vec4(1.f, 0.f, 0.f, 1.f), m_matColliderWorld, false, 0.f);
	}
	else
	{
		DrawDebugCube(Vec4(0.f, 1.f, 0.f, 1.f), m_matColliderWorld, false, 0.f);
	}
}


void CCollider3D::BeginOverlap(CCollider3D* POther)
{
	++m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(this, POther->GetOwner(), POther);
	}
}

void CCollider3D::Overlap(CCollider3D* POther)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->Overlap(this, POther->GetOwner(), POther);
	}
}

void CCollider3D::EndOverlap(CCollider3D* POther)
{
	--m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->EndOverlap(this, POther->GetOwner(), POther);
	}
}

void CCollider3D::BeginOverlap(CColliderRay* POther)
{
	++m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(POther, POther->GetOwner(), this);
	}
}

void CCollider3D::Overlap(CColliderRay* POther)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->Overlap(POther, POther->GetOwner(), this);
	}
}

void CCollider3D::EndOverlap(CColliderRay* POther)
{
	--m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->EndOverlap(POther, POther->GetOwner(), this);
	}
}

void CCollider3D::BeginOverlap(CLandScape* POther)
{
	++m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(this, POther->GetOwner(), POther);
	}
}

void CCollider3D::Overlap(CLandScape* POther)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->Overlap(this, POther->GetOwner(), POther);
	}
}

void CCollider3D::EndOverlap(CLandScape* POther)
{
	--m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->EndOverlap(this, POther->GetOwner(), POther);
	}
}

void CCollider3D::BeginOverlap(CMeshCollider* POther)
{
	++m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(this, POther->GetOwner(), POther);
	}
}

void CCollider3D::Overlap(CMeshCollider* POther)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->Overlap(this, POther->GetOwner(), POther);
	}
}

void CCollider3D::EndOverlap(CMeshCollider* POther)
{
	--m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->EndOverlap(this, POther->GetOwner(), POther);
	}
}

void CCollider3D::SetTrigger(bool _true)
{
	if (_true)
	{
		m_Status |= TRIGGER;
	}
	else
	{
		m_Status &= ~TRIGGER;
	}
}

void CCollider3D::SetIndependentRot(bool _true)
{
	if (_true)
	{
		m_Status |= INDEPENDENT_ROT;
	}
	else
	{
		m_Status &= ~INDEPENDENT_ROT;
	}
}

void CCollider3D::SaveComponent(FILE* PFile)
{
	(void)fwrite(&m_Offset, sizeof(Vec3), 1, PFile);
	(void)fwrite(&m_Scale, sizeof(Vec3), 1, PFile);
	(void)fwrite(&m_FinalPos, sizeof(Vec3), 1, PFile);
	(void)fwrite(&m_IndependentScale, sizeof(bool), 1, PFile);

	// 추가 필요 저장 데이터
	(void)fwrite(&m_Status, sizeof(int), 1, PFile);
}

void CCollider3D::LoadComponent(FILE* PFile)
{
	(void)fread(&m_Offset, sizeof(Vec3), 1, PFile);
	(void)fread(&m_Scale, sizeof(Vec3), 1, PFile);
	(void)fread(&m_FinalPos, sizeof(Vec3), 1, PFile);
	(void)fread(&m_IndependentScale, sizeof(bool), 1, PFile);

	// 추가 필요 로드 데이터
	(void)fread(&m_Status, sizeof(int), 1, PFile);
}
