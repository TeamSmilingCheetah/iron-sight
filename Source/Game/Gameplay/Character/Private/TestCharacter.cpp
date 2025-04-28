#include "pch.h"
#include "Game\Gameplay\Character\Public\TestCharacter.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"

#include "Game/Gameplay/Character/Public/EnemyVisionScript.h"

TestCharacter::TestCharacter()
	:EnemyController(SCRIPT_TYPE::TESTCHARACTER)
{
	SetParentScriptType(SCRIPT_TYPE::ENEMYCONTROLLER);
}

TestCharacter::~TestCharacter()
{
}

void TestCharacter::Begin()
{
	// 사운드 등록
	m_BulletSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\ak_reverb.wav", L"Sound\\ak_reverb.wav");

	// 자식 오브젝트(시야용 오브젝트)탐색후 등록
	const vector<CGameObject*>&  vecChild = GetOwner()->GetChild();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		vector<CScript*> vecScript = vecChild[i]->GetScripts();

		CScript* VisionScript = GetScriptWithParentType(vecChild[i], ENEMYVISION);
		if (nullptr == VisionScript)
			continue;
		else
		{
			m_VisionObj = vecChild[i];
			m_VisionScript = static_cast<EnemyVisionScript*>(VisionScript);
			break;
		}
	}

	m_State = Enemy_State::None;
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

	// 이동처리
	EnemyController::Tick();
}

void TestCharacter::DemageCalcul(int _Demage)
{
	m_HP -= _Demage;

	// 0보다 낮으면 사망
	if (m_HP < 0.f)
	{
		m_HP = 0.f;

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
			// 공격
			// 추후 EnemyGunController를 만들어 사용



		}
		// 공격 거리에 보이지 않으면 추격
		else
		{
			m_State = Enemy_State::Trace;
		}


	}
}

void TestCharacter::SaveComponent(FILE* _File)
{
}

void TestCharacter::LoadComponent(FILE* _File)
{
}
