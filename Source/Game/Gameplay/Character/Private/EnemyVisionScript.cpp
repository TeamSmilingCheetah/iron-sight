#include "pch.h"
#include "Game/Gameplay/Character/Public/EnemyVisionScript.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Base/components.h"

EnemyVisionScript::EnemyVisionScript()
	: CScript(SCRIPT_TYPE::ENEMYVISION)
	  , m_TargetType(SCRIPT_TYPE::PLAYERSCRIPT)
	  , m_VisionRange(5000.f)
	  , m_AtkRange(2000.f)
	  , m_AtkRgmax(3000.f)
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
	// Collider가 존재해야한다(없으면 종료)
	FRayCollider* pRay = GetOwner()->RayCollider();
	FBoxCollider* p3DCol = GetOwner()->BoxCollider();
	if (pRay == nullptr || p3DCol == nullptr)
	{
		return;
	}


	// 탐색개시

	// 시야지점에 타겟이 아무것도 없다면
	if (m_SetScript.empty())
	{
		m_Vision = false;
		m_RayTarget = false;
		m_RayAtkRg = false;
		// 문제되면 여기서 레이방향을 원점으로 되돌리는 코드 추가

		return; // 타겟이 없으면 여기서 종료
	}
	else
	{
		m_Vision = true;
	}
	// 시야에 타겟이 존재한다면
	CScript* pScript = *m_SetScript.begin(); // 지금 당장은 타겟이 하나기에 이렇게 설정
	Vec3 RayPos = pRay->GetRayFinalPos();
	Vec3 TargetPos = pScript->GetOwner()->Transform()->GetRelativePos();
	float Length = pRay->GetTargetInfoRef().Length;

	// Ray가 타겟을 찾은지 확인
	if (m_RayTarget)
	{
		// 사거리 내에 있는지 확인
		float Length = pRay->GetTargetInfoRef().Length;
		if (Length <= m_AtkRange)
		{
			m_RayAtkRg = true;
		}
		// 사거리 내에 있던 상태이며 일정거리가 벌어지게되면
		if (m_RayAtkRg && Length >= m_AtkRgmax)
		{
			m_RayAtkRg = false;
		}
		m_RayAtkRg = true;
	}

	// 레이 방향을 타겟 방향으로 설정
	Vec3 RayVector = TargetPos - RayPos;
	pRay->SetRayDir(RayVector);
	pRay->SetRayLength(m_VisionRange);
}

void EnemyVisionScript::BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::RayCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		// 타겟으로 지정된 스크립트가 들어있나 확인
		CScript* targetScript = GetScriptWithType(OtherObject, m_TargetType);
		if (targetScript == nullptr)
		{
			// 없으면 처리x
			return;
		}

		// 타겟을 발견으로 침
		m_RayTarget = true;
	}

	if (InCollider->GetColliderType() == EColliderType::BoxCollider &&
	InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		// 타겟으로 지정된 스크립트가 들어있나 확인
		CScript* targetScript = GetScriptWithType(OtherObject, m_TargetType);
		if (targetScript == nullptr)
		{
			// 없으면 처리x
			return;
		}

		// 여기에 들어오면 시야 체크대상등록
		m_SetScript.insert(targetScript);
	}
}

void EnemyVisionScript::Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::RayCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		// 혹시 모를 사태에 대비 무조건 true로
		m_RayTarget = true;
	}
}

void EnemyVisionScript::EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::RayCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		// 타겟으로 지정된 스크립트가 들어있나 확인
		CScript* targetScript = GetScriptWithType(OtherObject, m_TargetType);
		if (targetScript == nullptr)
		{
			// 없으면 처리x
			return;
		}

		// 타겟이제 놓친걸로 침
		m_RayTarget = false;
	}

	if (InCollider->GetColliderType() == EColliderType::BoxCollider &&
	InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		// 타겟으로 지정된 스크립트가 들어있나 확인
		CScript* targetScript = GetScriptWithType(OtherObject, m_TargetType);
		if (targetScript == nullptr)
		{
			// 없으면 처리x
			return;
		}

		// 시야체크 대상이라면 해제해줌
		m_SetScript.erase(targetScript);

		// 시야지점에 타겟이 아무것도 없다면 전부 false로 강제 전환
		if (m_SetScript.empty())
		{
			m_Vision = false;
			m_RayTarget = false;
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
