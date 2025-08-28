#include "pch.h"
#include "Game/Gameplay/Character/Public/GroundCheck.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Physics/RayCollider.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

#include "Engine/System/Public/Manager/CTimeMgr.h"

GroundCheck::GroundCheck()
	: CScript(SCRIPT_TYPE::GROUNDCHECK)
{
}

GroundCheck::~GroundCheck()
{
}

void GroundCheck::BeginOverlap(IColliderBase* InSelf, IColliderBase* InOther)
{
	// Ray만 존재 가정
	assert(InSelf->GetColliderType() == EColliderType::RayCollider);

	LOG_INFO_F("[GroundCheck]BeginOverlap");
}

void GroundCheck::Overlap(IColliderBase* InSelf, IColliderBase* InOther)
{
	// Ray만 존재 가정
	assert(InSelf->GetColliderType() == EColliderType::RayCollider);

	FRayCollider* ray = static_cast<FRayCollider*>(InSelf);
	float offset = ray->GetOffset().y;

	float RayStartY = m_PlayerScript->Transform()->GetRelativePos().y + offset;
	float RayDist = ray->GetClosestHitDistance();
	float VelocityY = m_PlayerScript->GetPlayerVelocity().y;

	//LOG_INFO_F("[GroundCheck] Overlap : Ray Start Y {} - Ray Distance {} = Expected Ground {}  ", RayStartY, RayDist, RayStartY - RayDist);
	//LOG_INFO_F("[GroundCheck] Overlap : GroundState = {}", (int)m_PlayerScript->GetGroundState());

	// 절대 거리가 아니라, 플레이어의 y축 속도와 애니메이션 시간으로 기준 거리를 설정

	// 500(ray offset) 보다 가까우면 = Ground
	const float eps = 0.01f;
	const float RayOffset = 500.f;
	if (RayDist < RayOffset + eps)
	{
		Vec3 vPos = m_PlayerScript->Transform()->GetRelativePos();
		vPos.y += RayOffset - RayDist;
		m_PlayerScript->Transform()->SetRelativePos(vPos);

		m_PlayerScript->SetGroundState(GROUND_STATE::OnGround);
	}
	else
	{
		if (VelocityY > 0.f)
		{
			m_PlayerScript->SetGroundState(GROUND_STATE::InAir);
		}
		else
		{
			// 절댓값으로 계산
			VelocityY = -VelocityY;

			const float GravityAccel = m_PlayerScript->GetGravityAccel();
			constexpr float TimeThreshold = 0.67f;

			// 속도를 적분하여 땅에 떨어질 때까지의 시간을 역추적
			const float LandingDuration = (-VelocityY + sqrt(VelocityY * VelocityY + 2 * GravityAccel * (RayDist - RayOffset))) / GravityAccel;

			//LOG_INFO_F("V = {}, D = {}, DT = {}, LandingDuration = {}", VelocityY, RayDist - RayOffset, DT, LandingDuration);

			if (LandingDuration < TimeThreshold)
			{

				m_PlayerScript->SetGroundState(GROUND_STATE::Landing);
			}
			else
			{
				m_PlayerScript->SetGroundState(GROUND_STATE::InAir);
			}
		}

		
	}
}

void GroundCheck::EndOverlap(IColliderBase* InSelf, IColliderBase* InOther)
{
	// Ray만 존재 가정
	assert(InSelf->GetColliderType() == EColliderType::RayCollider);

	LOG_INFO_F("[GroundCheck]EndOverlap");

	m_PlayerScript->SetGroundState(GROUND_STATE::InAir);
}

void GroundCheck::Begin()
{
	// 플레이어 스크립트 캐싱
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(GetOwner()->GetParent(), SCRIPT_TYPE::PLAYERSCRIPT));
}

void GroundCheck::Tick()
{
}

void GroundCheck::SaveComponent(FILE* _File)
{
}

void GroundCheck::LoadComponent(FILE* _File)
{
}

