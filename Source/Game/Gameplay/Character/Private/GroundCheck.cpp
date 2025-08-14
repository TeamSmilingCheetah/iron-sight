#include "pch.h"
#include "Game/Gameplay/Character/Public/GroundCheck.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Physics/RayCollider.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

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

	LOG_INFO_F("[GroundCheck] Overlap : Ray Start Y {} - Ray Distance {} = Expected Ground {}  ", RayStartY, RayDist, RayStartY - RayDist);

	// TODO(Ssio) : 속도 아래인것도 체크해줘야 하나?
	if (m_PlayerScript->GetPlayerVelocity().y > 0.f)
	{
		return;
	}

	// 100보다 멀리 떨어져 있으면 = Landing
	if (RayDist > 400.f)
	{
		m_PlayerScript->SetGroundState(GROUND_STATE::Landing);
	}

	// 100보다 가까우면 = Ground
	else
	{
		m_PlayerScript->SetGroundState(GROUND_STATE::OnGround);
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

