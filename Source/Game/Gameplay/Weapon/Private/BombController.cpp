#include "pch.h"
#include "Game/Gameplay/Weapon/Public/BombController.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Character/Public/EnemyController.h"

BombController::BombController()
	: CScript(SCRIPT_TYPE::BOMBSCRIPT)
	, m_DMG(100.f)
	, m_AccTime(0.f)
	, m_MaxLength(0.f)
	, m_WeaponOwner(nullptr)
{
}

BombController::~BombController()
{
}

void BombController::Begin()
{
	float x = Collider3D()->GetScale().x / 2.f;
	float y = Collider3D()->GetScale().y / 2.f;

	m_MaxLength = sqrt(x * x + y * y);
}

void BombController::Tick()
{
	m_AccTime += DT;

	if (0.2f < m_AccTime)
	{
		DestroyObject(GetOwner());
	}
}


void BombController::BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	if (_OtherCollider->IsTrigger())
	{
		return;
	}

	Vec3 vOtherPos = _OtherObject->Transform()->GetRelativePos();
	Vec3 vOwnPos = Transform()->GetRelativePos();

	// 폭발이 발생한 지점과 피격 오브젝트의 거리를 구한다.
	float fDis = (vOtherPos - vOwnPos).Length();
	float fDisRatio = 1.f - (fDis / m_MaxLength);

	float finalDMG = 0.f;



	// 최대 데미지가 적용되는 범위를 보장해준다.
	if (0.8f < fDisRatio)
	{
		finalDMG = m_DMG;
	}
	else
	{
		// 거리 비율에 따라 데미지를 결정한다.
		finalDMG = fDisRatio* m_DMG;
	}

	// 최소 데미지 20으로 보장해준다.
	if (finalDMG < 20.f)
	{
		finalDMG = 20.f;
	}


	CScript* pScript = GetScriptWithType(_OtherObject, SCRIPT_TYPE::PLAYERSCRIPT);
	// 피격된 대상이 플레이어일 경우
	if (pScript != nullptr)
	{
		// 플레이어 스크립트 접근
		PlayerCharacter* PlayerScript = (PlayerCharacter*)pScript;
		PlayerScript->DemageCalcul(m_WeaponOwner, GetOwner(), finalDMG);


		// 강제 종료
		return;
	}

	pScript = GetScriptWithType(_OtherObject, SCRIPT_TYPE::ENEMYCONTROLLER);
	// 적일 경우(적들은 EnemyController를 부모로 보유)
	if (pScript != nullptr)
	{
		// 적 스크립트 접근
		EnemyController* EnemyScript = (EnemyController*)pScript;
		EnemyScript->DemageCalcul(m_WeaponOwner, GetOwner(), finalDMG);

		// 강제 종료
		return;
	}
}

void BombController::Overlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}

void BombController::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}




void BombController::SaveComponent(FILE* _File)
{
}

void BombController::LoadComponent(FILE* _File)
{
}
