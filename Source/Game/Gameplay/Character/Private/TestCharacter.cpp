#include "pch.h"
#include "Game\Gameplay\Character\Public\TestCharacter.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"

#include "Game/Gameplay/Character/Public/EnemyVisionScript.h"
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"

TestCharacter::TestCharacter()
	:EnemyController(SCRIPT_TYPE::TESTCHARACTER)
	, m_AttackDeley(1.f)
	, m_AttackTime(1.f)
	, m_IsAttack(false)
{
}

TestCharacter::~TestCharacter()
{
}

void TestCharacter::Begin()
{
	// 자식 오브젝트(시야용 오브젝트)탐색후 등록
	const vector<CGameObject*>&  vecChild = GetOwner()->GetChild();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		vector<CScript*> vecScript = vecChild[i]->GetScripts();

		CScript* VisionScript = GetScriptWithType(vecChild[i], SCRIPT_TYPE::ENEMYVISION);
		if (nullptr == VisionScript)
			continue;
		else
		{
			m_VisionObj = vecChild[i];
			m_VisionScript = static_cast<EnemyVisionScript*>(VisionScript);
			break;
		}
	}


	// 기존 총기 복사하여 가지고 있기
	CGameObject* AKMGun =  CLevelMgr::GetInst()->FindObjectByName(L"AKM");
	if (nullptr != AKMGun)
	{
		CGameObject* AKMClone = AKMGun->Clone();
		m_CurWeapon = AKMClone;
		// 해당 아이템의 스크립트 확인하여 gun스크립트 추출
		// 기존 플레이어 추출 방식 사용
		m_WeaponScript = static_cast<WeaponController*>(AKMClone->GetScript(SCRIPT_TYPE::GUNSCRIPT));
		if (m_WeaponScript == nullptr)
		{
			m_WeaponScript = static_cast<WeaponController*>(AKMClone->GetScript(SCRIPT_TYPE::THROWABLESCRIPT));
			m_WeaponType = SCRIPT_TYPE::THROWABLESCRIPT;
		}
		else
			m_WeaponType = SCRIPT_TYPE::GUNSCRIPT;

		assert(m_WeaponScript != nullptr);		// WeaponScript가 있다는 가정

		// 총기 설정
		m_WeaponScript->SetEquippedOwner(GetOwner());
		m_WeaponScript->SetEquip(true);
		m_WeaponScript->SetEquipEnemy(true);
		// 부착
		AttachItem(AKMClone, GetOwner(), Vec3(0.f), Vec3(0.f,-90.f,0.f));
	}

	m_State = Enemy_State::None;

	EnemyController::Begin();
}

void TestCharacter::Tick()
{
	// 아무것도 아닌 상태일 시
	if (m_State == Enemy_State::None)
	{
		None();
	}

	// 추적 상태일 시
	if (m_State == Enemy_State::Trace)
	{
		Trace();
	}

	// 공격 상태일 시
	if (m_State == Enemy_State::Attack)
	{
		Attack();
	}

	// 사망 상태 시
	if (m_State == Enemy_State::Death)
	{
		Death();
	}

	// 이전상태가 뭔지 저장
	m_PrevState = m_State;

	// 부모 틱 처리(이동, 키 입력 처리)
	EnemyController::Tick();
}

void TestCharacter::DemageCalcul(CGameObject* _AtkObject, float _Demage)
{
	m_HP -= _Demage;

	// 0보다 낮으면 사망
	if (m_HP < 0)
	{
		m_HP = 0;

		m_State = Enemy_State::Death;
		DeathEntry();
	}
}

void TestCharacter::DeathEntry()
{

}

void TestCharacter::None()
{
	// 비전 오브젝트에서 체크사항 확인
	if (m_VisionObj != nullptr)
	{
		// 이동 정지
		m_InputMoveDir = Vec3(0.f);
		m_InputMoveForce = 0.f;

		// 뷰에 보이는 상태
		if (m_VisionScript->IsTargetView())
		{
			m_State = Enemy_State::Trace;
		}
	}
}


void TestCharacter::Trace()
{
	// 비전 오브젝트에서 체크사항 확인
	if (m_VisionObj != nullptr)
	{
		// 뷰에 보이는 상태
		if (m_VisionScript->IsTargetRange())
		{
			// 공격상태로 전환, 이동중지
			m_State = Enemy_State::Attack;
			m_InputMoveDir = Vec3(0.f);
			m_InputMoveForce = 0.f;
		}
		else if (m_VisionScript->IsTargetView())
		{
			// 추적 대상을 향해 이동
			set<CScript*> Script = m_VisionScript->GetTargetScript();

			// 버그에 대비하여 만일 충돌된 대상이 없다면 강제적으로 전환
			// 더미데이터로 내버려둠
			if (Script.empty())
			{

			}

			CScript* pScript = *Script.begin();
			Vec3 TargetPos = pScript->Transform()->GetRelativePos();
			Vec3 pPos = Transform()->GetRelativePos();

			Vec3 MoveDir = TargetPos - pPos;
			MoveDir.y = 0.f;		// 3차원 이동안할거라 y는 0으로 고정
			MoveDir.Normalize();
			m_InputMoveDir = MoveDir;
			m_InputMoveForce = 100.f;
		}
		else
		{
			// 여기까지오면 보지 못하니까 None으로 전환
			m_State = Enemy_State::None;
		}
	}
}

void TestCharacter::Death()
{
	//알아보기 쉽게 빙글빙글 회전
	Vec3 vRot = Transform()->GetRelativeRotation();
	vRot.y += DT * 360;
	Transform()->SetRelativeRotation(vRot);
}

void TestCharacter::Attack()
{
	// 비전 오브젝트에서 체크사항 확인
	if (m_VisionObj != nullptr)
	{
		// 공격 거리에 보이는 상태
		if (m_VisionScript->IsTargetRange())
		{
			// 설정된 딜레이만큼 사격, 시간이 0이하가 되면 사격중지하고 딜레이까지 대기

			// 우선 재장전이 필요한지 확인
			bool needReload = false;
			bool isReloading = false;

			if (m_WeaponType == SCRIPT_TYPE::GUNSCRIPT)
			{
				GunController* pGunScript = static_cast<GunController*>(m_WeaponScript);

				// 현재 잔탄량과 재장전 상태 확인
				int Bullet = pGunScript->GetCurRound();
				isReloading = pGunScript->IsReload();

				// 잔탄량이 0이하이고 현재 재장전 중이 아니면 재장전 필요
				if (Bullet <= 0 && !isReloading)
				{
					needReload = true;
				}
			}

			// 재장전이 필요하면 재장전 실행
			if (needReload)
			{
				KeyPush(KEY::R);
				return;  // 재장전 시작했으므로 여기서 함수 종료
			}

			// 재장전 중이면 공격하지 않음
			if (isReloading)
			{
				return;  // 재장전 중이므로 여기서 함수 종료
			}

			// 공격 로직 - 재장전 중이 아닐 때만 실행됨
			// 공격 딜레이중
			if (m_IsAttack && m_AttackDeley > m_AttackTime)
			{
				// 공격을 위해 좌클을 누름
				KeyPush(KEY::LBTN);
				m_AttackTime += DT;
			}
			// 공격 딜레이 종료시점
			else if (m_IsAttack)
			{
				m_IsAttack = false;
			}
			// 공격 대기 상태
			else
			{
				m_AttackTime -= DT;

				// 공격재사용 대기시간이 지나면 다시 공격
				if (m_AttackTime <= 0)
				{
					m_IsAttack = true;
					m_AttackTime = 0.f;
				}
			}
		}
		// 공격 거리에 보이지 않으면 추격
		else
		{
			m_State = Enemy_State::Trace;
		}
	}

}

void TestCharacter::KeyInputProcessing()
{
	// 키입력에 따른 처리

	// 무기를 들고 있고 좌클 입력이 들어올 경우
	if (m_CurWeapon != nullptr && AIKEY_TAP(KEY::LBTN))
	{
		m_WeaponScript->SetCurKey(KEY::LBTN);
		m_WeaponScript->SetCurKeyState(KEY_STATE::TAP);
	}

	// 좌클 유지
	if (m_CurWeapon != nullptr && AIKEY_PRESSED(KEY::LBTN))
	{
		// 총을 사용한다.
		m_WeaponScript->SetCurKey(KEY::LBTN);
		m_WeaponScript->SetCurKeyState(KEY_STATE::PRESSED);

	}

	// 좌클 해제
	if (m_CurWeapon != nullptr && AIKEY_RELEASED(KEY::LBTN))
	{
		// 총기
		m_WeaponScript->SetCurKey(KEY::LBTN);
		m_WeaponScript->SetCurKeyState(KEY_STATE::RELEASED);
	}

	// R 입력
	if (m_CurWeapon != nullptr && AIKEY_TAP(KEY::R) || AIKEY_PRESSED(KEY::R))
	{
		// 재장전
		m_WeaponScript->SetCurKey(KEY::R);
		m_WeaponScript->SetCurKeyState(KEY_STATE::TAP);
	}
}

void TestCharacter::SaveComponent(FILE* _File)
{
}

void TestCharacter::LoadComponent(FILE* _File)
{
}
