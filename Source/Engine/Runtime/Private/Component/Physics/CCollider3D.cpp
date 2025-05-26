#include "pch.h"
#include "Runtime/Public/Component/Physics/CCollider3D.h"

#include "Runtime/Public/Component/Script/CScript.h"
#include "Runtime/Public/Component/Transform/CTransform.h"
#include "Runtime/Public/Component/Physics/CColliderRay.h"
#include "Runtime/Public/Component/Rendering/CLandScape.h"


CCollider3D::CCollider3D()
	:CComponent(COMPONENT_TYPE::COLLIDER3D)
	, m_IndependentScale(false)
	, m_OverlapCount(0)
	, m_Offset(Vec3(0.f))
	, m_Scale(Vec3(0.f))
	, m_State(ACTIVE)
	, m_Status(0)
{
}

CCollider3D::CCollider3D(const CCollider3D& _Origin)
	: CComponent(_Origin)
	, m_Offset(_Origin.m_Offset)
	, m_Scale(_Origin.m_Scale)
	, m_FinalPos(_Origin.m_FinalPos)
	, m_IndependentScale(_Origin.m_IndependentScale)
	, m_OverlapCount(0)
	, m_State(_Origin.m_State)
	, m_Status(_Origin.m_Status)
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

CCollider3D::~CCollider3D()
{
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
			Matrix matScaleInv = XMMatrixInverse(nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));			
			m_matColliderWorld = matScale * matRot * matTrans * matScaleInv * ownerMatNoRot;
		}
		else
		{
			Matrix ownerTrans = XMMatrixTranslationFromVector(GetOwner()->Transform()->GetWorldPos() * GetOwner()->Transform()->GetWorldScale());
			m_matColliderWorld = matScale * matRot * matTrans * ownerMatNoRot;
		}
	}
	else
	{
		if (m_IndependentScale)
		{
			Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
			Matrix matScaleInv = XMMatrixInverse(nullptr, XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z));
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


void CCollider3D::BeginOverlap(CCollider3D* _Other)
{
	++m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(this, _Other->GetOwner(), _Other);
	}
}

void CCollider3D::Overlap(CCollider3D* _Other)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->Overlap(this, _Other->GetOwner(), _Other);
	}
}

void CCollider3D::EndOverlap(CCollider3D* _Other)
{
	--m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->EndOverlap(this, _Other->GetOwner(), _Other);
	}
}

void CCollider3D::BeginOverlap(CColliderRay* _Other)
{
	++m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(_Other, _Other->GetOwner(), this);
	}
}

void CCollider3D::Overlap(CColliderRay* _Other)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->Overlap(_Other, _Other->GetOwner(), this);
	}
}

void CCollider3D::EndOverlap(CColliderRay* _Other)
{
	--m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->EndOverlap(_Other, _Other->GetOwner(), this);
	}
}

void CCollider3D::BeginOverlap(CLandScape* _Other)
{
	++m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(this, _Other->GetOwner(), _Other);
	}
}

void CCollider3D::Overlap(CLandScape* _Other)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->Overlap(this, _Other->GetOwner(), _Other);
	}
}

void CCollider3D::EndOverlap(CLandScape* _Other)
{
	--m_OverlapCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->EndOverlap(this, _Other->GetOwner(), _Other);
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

void CCollider3D::SetIndependetRot(bool _true)
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

void CCollider3D::SaveComponent(FILE* _File)
{
	fwrite(&m_Offset, sizeof(Vec3), 1, _File);
	fwrite(&m_Scale, sizeof(Vec3), 1, _File);
	fwrite(&m_FinalPos, sizeof(Vec3), 1, _File);
	fwrite(&m_IndependentScale, sizeof(bool), 1, _File);

	// 추가 필요 저장 데이터
	//fwrite(&m_Status, sizeof(int), 1, _File);
}

void CCollider3D::LoadComponent(FILE* _File)
{
	fread(&m_Offset, sizeof(Vec3), 1, _File);
	fread(&m_Scale, sizeof(Vec3), 1, _File);
	fread(&m_FinalPos, sizeof(Vec3), 1, _File);
	fread(&m_IndependentScale, sizeof(bool), 1, _File);

	// 추가 필요 로드 데이터
	//fread(&m_Status, sizeof(int), 1, _File);
}
