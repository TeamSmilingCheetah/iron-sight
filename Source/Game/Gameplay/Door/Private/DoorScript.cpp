#include "pch.h"
#include "Game/Gameplay/Door/Public/DoorScript.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

#include "Game/Gameplay/Character/Public/InteractionHandler.h"

DoorScript::DoorScript()
	: InteractableScript(SCRIPT_TYPE::DOORSCRIPT)
	, m_Opened(false)
	, m_CurClipAccTime(0.f)
	, m_Duration(1.f)
{
	m_InteractionDesc = L"문 열기";
}

DoorScript::~DoorScript()
{
}

void DoorScript::Init()
{
}

void DoorScript::Begin()
{
	m_ChangeAngle = Transform()->GetRelativeRotation().y;
}

void DoorScript::Tick()
{
	if (m_Rotating)
	{
		m_CurClipAccTime += DT;

		if (m_CurClipAccTime > m_Duration)
			m_CurClipAccTime = m_Duration;

		float ratio = m_CurClipAccTime / m_Duration;

		if (m_Opened)
			ratio = 1 - ratio;

		Transform()->SetRelativeRotation(Vec3(0.f, m_OrigAngle + m_ChangeAngle * ratio, 0.f));

		if (m_CurClipAccTime == m_Duration)
		{
			m_Rotating = false;
			m_Opened = !m_Opened;

			if (m_Opened)
				m_InteractionDesc = L"문 닫기";
			else
				m_InteractionDesc = L"문 열기";
		}
	}
}

void DoorScript::SaveComponent(FILE* _File)
{
}

void DoorScript::LoadComponent(FILE* _File)
{
}

void DoorScript::Interact(InteractionHandler* _Handler)
{
	if (m_Rotating)
		return;

	Vec3 vRot = Transform()->GetRelativeRotation();

	// y축 회전만 적용되었다는 가정
	assert(vRot.x == 0.f && vRot.z == 0.f);

	Vec3 vPlayerPos = _Handler->GetOwner()->GetParent()->Transform()->GetRelativePos();
	Vec3 vDoorPos = Transform()->GetRelativePos();

	// 문은 회전이 없을 때 x축으로 향한다고 하자.
	// 즉, 위에서 봤을 때 o====== 형태

	float angle = XMConvertToRadians(vRot.y);

	Vec2 vDoorDir = Vec2(cos(angle), sin(angle));
	Vec2 vDoorToPlayer = Vec2((vPlayerPos - vDoorPos).x, (vPlayerPos - vDoorPos).z);
	vDoorToPlayer.Normalize();

	if (m_Opened)
		m_ChangeAngle = vRot.y - m_OrigAngle;
	else
		m_ChangeAngle = vDoorToPlayer.Cross(vDoorDir).y > 0.f ? -90.f : 90.f;

	m_CurClipAccTime = 0.f;

	m_Rotating = true;
}
