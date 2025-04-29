#include "pch.h"
#include "Game/Gameplay/Projectile/Public/MissileProjectile.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/System/Public/Manager/CObjectPoolMgr.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/Runtime/Public/Component/Rendering/CDecal.h"

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
		m_LifeTime = 0.f;
		CObjectPoolMgr::GetInst()->ReturnObject(GetOwner());
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


void MissileProjectile::BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	

	if (_OtherObject->GetName() == L"DeathBox")
	{
		// Decal Prefab을 불러온다.
		Ptr<CPrefab> DecalPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\asd.pref", L"Prefab\\asd.pref");


		Vec3 vPos = _OtherObject->Transform()->GetRelativePos();
		Vec3 vScale = _OtherCollider->GetScale();

		CGameObject* pDecal = DecalPrefab->GetProtoObject();
		Vec3 vDecalRot = pDecal->Transform()->GetRelativeRotation();

		// Decal의 수명 설정
		pDecal->Decal()->SetLifeTime(6.f);

		// 충돌한 방향을 구한다.
		Vec3 vDir = CalcColiisionDir(_OtherObject, GetOwner());


		// x축에서 충돌
		if (vDir.x != 0)
		{
			// 위치 값 (해당 물체의 중심에서 크기의 반만큼 보정해준다)
			vPos.x = vPos.x - vDir.x * (vScale.x / 2.f);
			vPos.z = Transform()->GetRelativePos().z;
			vPos.y = Transform()->GetRelativePos().y;

			// 회전 값
			vDecalRot.x = 0.f;
			vDecalRot.y = 0.f;
			vDecalRot.z = 90.f;
		}
		// z축에서 충돌
		if (vDir.z != 0)
		{
			vPos.x = Transform()->GetRelativePos().x;
			vPos.z = vPos.z - vDir.z * (vScale.z / 2.f);
			vPos.y = Transform()->GetRelativePos().y;

			vDecalRot.x = 90.f;
			vDecalRot.y = 0.f;
			vDecalRot.z = 0.f;
		}
		// y축에서 충돌
		if (vDir.y != 0)
		{
			vPos.x = Transform()->GetRelativePos().x;
			vPos.z = Transform()->GetRelativePos().z;
			vPos.y = vPos.y - vDir.y * (vScale.y / 2.f);

			vDecalRot.x = 0.f;
			vDecalRot.y = 0.f;
			vDecalRot.z = 0.f;
		}

		pDecal->Transform()->SetRelativeRotation(vDecalRot);


		Instantiate(DecalPrefab, vPos, 0);

		// 총알 삭제
		//DestroyObject(GetOwner());
		CObjectPoolMgr::GetInst()->ReturnObject(GetOwner());
	}

}

void MissileProjectile::Overlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}

void MissileProjectile::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
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
