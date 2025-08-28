#include "pch.h"
#include "Game/Gameplay/Character/Public/EnemyVisionScript.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Base/components.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"

EnemyVisionScript::EnemyVisionScript()
	: CScript(SCRIPT_TYPE::ENEMYVISION)
	  , m_TargetType(SCRIPT_TYPE::PLAYERSCRIPT)
	  , m_VisionRange(6000.f)
	  , m_VisionAngle(120.f)
	  , m_AtkRange(4000.f)
	  , m_AtkRgmax(5000.f)
	  , m_Vision(false)
	  , m_RayTarget(false)
	  , m_RayAtkRg(false)
{
}

EnemyVisionScript::~EnemyVisionScript()
{
}


void EnemyVisionScript::Begin()
{
}

void EnemyVisionScript::Tick()
{
	// 타겟 대상 찾기
	if (m_TargetScript == nullptr)
	{
		CGameObject* player = CLevelMgr::GetInst()->FindObjectByName(L"Player");
		if (player != nullptr)
		{
			CScript* playerScript = GetScriptWithType(player, m_TargetType);
			if (playerScript != nullptr)
			{
				m_TargetScript = playerScript;
			}
		}

		// 못찾았으면 하면안됨
		if (m_TargetScript == nullptr)
		{
			return;
		}
	}

	// Collider가 존재해야한다(없으면 종료)
	// ray는 지금안쓰나 바라보는 방향 체크용 임시 적용 
	FRayCollider* pRay = GetOwner()->RayCollider();
	if (pRay == nullptr)
	{
		return;
	}

	Vec3 RayPos = pRay->GetFinalPosition();
	Vec3 TargetPos = m_TargetScript->GetOwner()->Transform()->GetRelativePos();

	// 시야 내에 있는지 체크
	VisionCheck();

	// 거리, 각도내에 있다면 ray방향 지정
	if (m_Vision)
	{
		Vec3 rayDirection = TargetPos - RayPos;
		rayDirection.Normalized();
		pRay->SetFinalDirection(rayDirection);
		pRay->SetLength(m_VisionRange);
	}

	// 탐색개시

	// 시야지점에 타겟이 아무것도 없다면
	//if (m_SetScript.empty())
	//{
	//	m_Vision = false;
	//	m_RayTarget = false;
	//	m_RayAtkRg = false;
	//	// 문제되면 여기서 레이방향을 원점으로 되돌리는 코드 추가
	//
	//	return; // 타겟이 없으면 여기서 종료
	//}
	//else
	//{
	//	m_Vision = true;
	//}
	//// 시야에 타겟이 존재한다면
	//CScript* pScript = *m_SetScript.begin(); // 지금 당장은 타겟이 하나기에 이렇게 설정
	//Vec3 RayPos = pRay->GetFinalPosition();
	//Vec3 TargetPos = pScript->GetOwner()->Transform()->GetRelativePos();
	//float Length = pRay->GetLength();
	//
	//// Ray가 타겟을 찾은지 확인
	//if (m_RayTarget)
	//{
	//	// 사거리 내에 있는지 확인
	//	if (Length <= m_AtkRange)
	//	{
	//		m_RayAtkRg = true;
	//	}
	//	// 사거리 내에 있던 상태이며 일정거리가 벌어지게되면
	//	if (m_RayAtkRg && Length >= m_AtkRgmax)
	//	{
	//		m_RayAtkRg = false;
	//	}
	//	m_RayAtkRg = true;
	//}
	//
	//// 레이 방향을 타겟 방향으로 설정
	//Vec3 RayVector = TargetPos - RayPos;
	//pRay->SetFinalDirection(RayVector);
	//pRay->SetLength(m_VisionRange);
}

void EnemyVisionScript::BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	//if (InCollider->GetColliderType() == EColliderType::RayCollider &&
	//	InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	//{
	//	FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
	//	CGameObject* OtherObject = OtherCollider->GetOwner();
	//
	//	// 타겟으로 지정된 스크립트가 들어있나 확인
	//	CScript* targetScript = GetScriptWithType(OtherObject, m_TargetType);
	//	if (targetScript == nullptr)
	//	{
	//		// 없으면 처리x
	//		return;
	//	}
	//
	//	// 타겟을 발견으로 침
	//	m_RayTarget = true;
	//}
	//
	//if (InCollider->GetColliderType() == EColliderType::BoxCollider &&
	//InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	//{
	//	FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
	//	CGameObject* OtherObject = OtherCollider->GetOwner();
	//
	//	// 타겟으로 지정된 스크립트가 들어있나 확인
	//	CScript* targetScript = GetScriptWithType(OtherObject, m_TargetType);
	//	if (targetScript == nullptr)
	//	{
	//		// 없으면 처리x
	//		return;
	//	}
	//
	//	// 여기에 들어오면 시야 체크대상등록
	//	m_SetScript.insert(targetScript);
	//}
}

void EnemyVisionScript::Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	//if (InCollider->GetColliderType() == EColliderType::RayCollider &&
	//	InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	//{
	//	// 혹시 모를 사태에 대비 무조건 true로
	//	m_RayTarget = true;
	//}
}

void EnemyVisionScript::EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	//if (InCollider->GetColliderType() == EColliderType::RayCollider &&
	//	InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	//{
	//	FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
	//	CGameObject* OtherObject = OtherCollider->GetOwner();
	//
	//	// 타겟으로 지정된 스크립트가 들어있나 확인
	//	CScript* targetScript = GetScriptWithType(OtherObject, m_TargetType);
	//	if (targetScript == nullptr)
	//	{
	//		// 없으면 처리x
	//		return;
	//	}
	//
	//	// 타겟이제 놓친걸로 침
	//	m_RayTarget = false;
	//}
	//
	//if (InCollider->GetColliderType() == EColliderType::BoxCollider &&
	//InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	//{
	//	FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
	//	CGameObject* OtherObject = OtherCollider->GetOwner();
	//
	//	// 타겟으로 지정된 스크립트가 들어있나 확인
	//	CScript* targetScript = GetScriptWithType(OtherObject, m_TargetType);
	//	if (targetScript == nullptr)
	//	{
	//		// 없으면 처리x
	//		return;
	//	}
	//
	//	// 시야체크 대상이라면 해제해줌
	//	m_SetScript.erase(targetScript);
	//
	//	// 시야지점에 타겟이 아무것도 없다면 전부 false로 강제 전환
	//	if (m_SetScript.empty())
	//	{
	//		m_Vision = false;
	//		m_RayTarget = false;
	//		m_RayAtkRg = false;
	//	}
	//}
}

void EnemyVisionScript::VisionCheck()
{
	// m_RayTarget은 원래 ray를 쏴서 보이는지 체크하였으나 충돌의존도를 줄이기 위해 강제로 변경시킴

	FRayCollider* pRay = GetOwner()->RayCollider();

	Vec3 VisionPos = pRay->GetFinalPosition();
	Vec3 targetPos = m_TargetScript->Transform()->GetWorldPos();

	// 거리 체크
	Vec3 diff = targetPos - VisionPos;
	float distance = diff.Length();

	// 기준 거리보다 멀다면 시야거리 내에 없음
	if (distance > m_VisionRange)
	{
		m_Vision = false;
		m_RayTarget = false;
		m_RayAtkRg = false;
		return;
	}

	// 각도 체크
	Vec3 VisionForward = GetOwner()->Transform()->GetWorldDir(DIR_TYPE::FRONT);
	VisionForward.Normalize();
	diff.Normalize();

	float dotProduct = VisionForward.Dot(diff);
	float angle = acosf(dotProduct) * 180.0f / XM_PI;

	// 기준 각도를 넘어선다면 시야 각도 내에 없음
	if (angle >= m_VisionAngle / 2.0f)
	{
		m_Vision = false;
		m_RayTarget = false;
		m_RayAtkRg = false;
		return;
	}

	// 여기에 도달하면 적은 대상을 찾음
	m_Vision = true;
	m_RayTarget = true;

	// 공격 사거리 체크
	if (!m_RayAtkRg)
	{
		// 현재 공격 모드가 아닐 시
		if (distance <= m_AtkRange)
		{
			m_RayAtkRg = true;
		}
	}
	else
	{
		// 현재 공격 모드
		if (distance >= m_AtkRgmax)
		{
			m_RayAtkRg = false;
		}
	}

}

void EnemyVisionScript::SaveComponent(FILE* _File)
{
}

void EnemyVisionScript::LoadComponent(FILE* _File)
{
}
