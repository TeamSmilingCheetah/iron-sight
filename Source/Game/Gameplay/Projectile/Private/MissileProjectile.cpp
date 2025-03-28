#include "pch.h"
#include "Game/Gameplay/Projectile/Public/MissileProjectile.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

MissileProjectile::MissileProjectile()
	: CScript(static_cast<UINT>(SCRIPT_TYPE::MISSILESCRIPT))
	, m_Velocity(Vec3(0.f, 500.f, 0.f))
	, m_Dir()
{
}

MissileProjectile::~MissileProjectile()
{
}

void MissileProjectile::Begin()
{
	// CAssetMgr 의 재질을 복사시킨 재질을 참조
	//Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl");
	//MeshRender()->SetMaterial(pMtrl, 0);
	//MeshRender()->GetDynamicMaterial(0);

	//Ptr<CTexture> pMissileTex = CAssetMgr::GetInst()->Load<CTexture>(
	//	L"Missile", L"Texture\\missile.png");
	//MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pMissileTex);
}

void MissileProjectile::Tick()
{
	m_LifeTime += DT;
	Vec3 vWorldPos = Transform()->GetRelativePos();

	vWorldPos += m_Dir * 20000.f * DT;

	Transform()->SetRelativePos(vWorldPos);

	if (1.f < m_LifeTime)
	{
		DestroyObject(GetOwner());
	}
}

void MissileProjectile::SaveComponent(FILE* _File)
{
	fwrite(&m_Velocity, sizeof(float), 1, _File);
}

void MissileProjectile::LoadComponent(FILE* _File)
{
	fread(&m_Velocity, sizeof(float), 1, _File);
}
