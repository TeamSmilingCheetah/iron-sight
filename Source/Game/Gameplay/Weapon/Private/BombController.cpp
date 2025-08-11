#include "pch.h"
#include "Game/Gameplay/Weapon/Public/BombController.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"
#include "Engine/System/Public/Manager/SoundManager.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Character/Public/EnemyController.h"

BombController::BombController()
	: CScript(SCRIPT_TYPE::BOMBSCRIPT)
	, m_TinnitusSoundIdx(-1)
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

	// 소리
	m_TinnitusSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\grenade_tinnitus.mp3");
}

void BombController::Tick()
{
	m_AccTime += DT;

	if (0.2f < m_AccTime)
	{
		DestroyObject(GetOwner());
	}
}


void BombController::BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::Collider3D &&
		InOtherCollider->GetColliderType() == EColliderType::Collider3D)
	{
		FBoxCollider* OtherCollider = reinterpret_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		if (OtherCollider->IsTrigger())
		{
			return;
		}

		Vec3 vOtherPos = OtherObject->Transform()->GetRelativePos();
		Vec3 vOwnPos = Transform()->GetRelativePos();
		Vec3 vClosestPoint = OtherCollider->GetClosestPoint();

		// 폭발이 발생한 지점과 피격 오브젝트의 거리를 구한다.
		float fDis = (vClosestPoint - vOwnPos).Length();
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


		CScript* pScript = GetScriptWithType(OtherObject, SCRIPT_TYPE::PLAYERSCRIPT);
		// 피격된 대상이 플레이어일 경우
		if (pScript != nullptr)
		{
			// 플레이어 스크립트 접근
			PlayerCharacter* PlayerScript = (PlayerCharacter*)pScript;
			PlayerScript->DamageCalcul(m_WeaponOwner, GetOwner(), finalDMG);

			// 플레이어의 소리를 먹먹하게 한다
			FSoundManager::GetInst()->ApplyMuffle(200.f, 10.f);

			// 이명소리 출력
			m_TinnitusSoundIdx = FSoundManager::GetInst()->Play3DSound(m_TinnitusSound, Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, false, false, m_TinnitusSoundIdx);

			// 강제 종료
			return;
		}

		pScript = GetScriptWithType(OtherObject, SCRIPT_TYPE::ENEMYCONTROLLER);
		// 적일 경우(적들은 EnemyController를 부모로 보유)
		if (pScript != nullptr)
		{
			// 적 스크립트 접근
			EnemyController* EnemyScript = (EnemyController*)pScript;
			EnemyScript->DamageCalcul(m_WeaponOwner, GetOwner(), finalDMG);

			// 강제 종료
			return;
		}
	}
}

void BombController::Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
}

void BombController::EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
}

void BombController::SaveComponent(FILE* _File)
{
}

void BombController::LoadComponent(FILE* _File)
{
}
