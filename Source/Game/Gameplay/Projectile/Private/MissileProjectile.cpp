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
	, m_Speed(0.f)
	, m_GravityAccel(450.f)
	, m_Mass(0.f)
{
	// 총알 종류에 따라 무게 설정
	m_Mass = 0.f;
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

	// 초기 속도 설정
	if (m_LifeTime <= DT)
	{
		m_Velocity = m_Dir * m_Speed;
	}

	// 중력 적용
	m_Velocity.y -= m_GravityAccel * DT;

	// 위치 업데이트
	vWorldPos += m_Velocity * DT;
	Transform()->SetRelativePos(vWorldPos);

	// 일정 시간 후 총알 삭제
	if (5.f < m_LifeTime)
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

void MissileProjectile::BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider)
{

}

void MissileProjectile::Overlap(CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider)
{
}

void MissileProjectile::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider)
{
}
