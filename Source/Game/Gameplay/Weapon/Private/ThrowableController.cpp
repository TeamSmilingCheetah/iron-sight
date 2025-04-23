#include "pch.h"
#include "Game/Gameplay/Weapon/Public/ThrowableController.h"

#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"


ThrowableController::ThrowableController()
	: WeaponController(SCRIPT_TYPE::THROWABLESCRIPT)
	, m_Velocity()
	, m_Dir()
	, m_Mass(0.f)
	, m_Speed(8000.f)
	, m_GravityAccel(1000.f)
	, m_AccTime(0.f)
	, m_TriggeredTime(6.f)
	, m_bGround(false)
	, m_bCanThrow(false)
	, m_bThrow(false)
	, m_bTrigger(false)
	
{
	m_Mass = 4.f;

	//wstring wName = GetOwner()->GetName();

	//if (wName == L"Grenade")
	//{
	//	m_TriggeredTime = 6.f;
	//}
	//if (wName == L"Smoke Grenade")
	//{
	//	m_TriggeredTime = 6.f;
	//}
	//if (wName == L"Flashbang")
	//{
	//	m_TriggeredTime = 1.f;
	//}
}

ThrowableController::~ThrowableController()
{
}

void ThrowableController::Begin()
{
	WeaponController::Begin();
}

void ThrowableController::Tick()
{
	// 소유주가 있다면 위치를 0으로 초기화
	if (m_EquippedOwner != nullptr)
	{
		Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
	}
	// 소유주가 없다면 return
	else
	{
		return;
	}

	PlayerCharacter* pPlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_EquippedOwner, (UINT)SCRIPT_TYPE::PLAYERSCRIPT));

	// 투척 준비
	if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::TAP)
	{
		m_bCanThrow = true;
		pPlayerScript->SetThrow(true);
		ClearKey();
	}


	// 투척 한다.
	if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::RELEASED)
	{
		// Player의 위치와 방향 정보
		Vec3 vPlayerPos = m_EquippedOwner->Transform()->GetRelativePos();

		// 부모를 없는 독립 개체로 바꿔준다.
		AddChild(nullptr, GetOwner());
		// 본래 Layer로 변경해준다.
		GetOwner()->SetLayerIdx(0);

		// 현재 Player 위치에 무기를 다시 생성시킨다.
		Vec3 vSpanwPos = vPlayerPos;
		vSpanwPos.y += 800.f;
		GetOwner()->Transform()->SetRelativePos(vSpanwPos);

		// Trigger를 꺼서 충돌을 진행할 수 있게 해준다.
		GetOwner()->Collider3D()->SetTrigger(false);

		m_ThrowAngle = 0.2f;
		m_Speed = 8000.f;
		m_bThrow = true;
		ClearKey();
	}


	// 투척 준비
	if (m_CurKey == KEY::RBTN && m_CurKeyState == KEY_STATE::TAP)
	{
		m_bCanThrow = true;
		pPlayerScript->SetThrow(true);
		ClearKey();
	}


	// 투척 한다.
  	if (m_CurKey == KEY::RBTN && m_CurKeyState == KEY_STATE::RELEASED)
	{
		// Player의 위치와 방향 정보
		Vec3 vPlayerPos = m_EquippedOwner->Transform()->GetRelativePos();

		// 부모를 없는 독립 개체로 바꿔준다.
		AddChild(nullptr, GetOwner());
		// 본래 Layer로 변경해준다.
		GetOwner()->SetLayerIdx(0);

		// 현재 Player 위치에 무기를 다시 생성시킨다.
		Vec3 vSpanwPos = vPlayerPos;
		vSpanwPos.y += 800.f;
		GetOwner()->Transform()->SetRelativePos(vSpanwPos);
		 
		// Trigger를 꺼서 충돌을 진행할 수 있게 해준다.
		GetOwner()->Collider3D()->SetTrigger(false);

		m_ThrowAngle = 1.f;
		m_Speed = 4000.f;
		m_bThrow = true;
		ClearKey();
	}


	// 
	if (m_CurKey == KEY::R && m_CurKeyState == KEY_STATE::TAP)
	{
		m_bTrigger = true;

	}


	if (m_bTrigger)
	{
		Triggered();
	}

	if (m_bThrow)
	{
		Throw();
	}
}

void ThrowableController::Triggered()
{
	m_AccTime += DT;

	// 시간이 지나면 작동한다.
	if (m_TriggeredTime < m_AccTime)
	{
		if (m_EquippedOwner != nullptr)
		{
			CGameObject* pPlayer = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
			PlayerCharacter* pPlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(pPlayer, (UINT)SCRIPT_TYPE::PLAYERSCRIPT));
			pPlayerScript->SetThrowBoom(true);
		}
		DestroyObject(GetOwner());
	}
}

void ThrowableController::Throw()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRotation();

	Collider3D()->SetTrigger(false);
	m_bTrigger = true;

	// 향할 초기 방향을 구한다.
	if (m_EquippedOwner != nullptr)
	{
		m_Dir = GetFireDir();
		m_Dir.y += m_ThrowAngle;
		m_Velocity = m_Dir * m_Speed;
	}
	
	// 방향을 구한뒤 소유주를 삭제한다.
	m_EquippedOwner = nullptr;

	// 감속
	float dragCoeff = 2.f;
	Vec3 dragAccel = -dragCoeff * m_Velocity / m_Mass;



	float veloLength = m_Velocity.Length();



	// 중력을 적용한다
	if(!m_bGround)
		m_Velocity.y -= m_GravityAccel * m_Mass * DT;

		

	// 특정 속도 이하가 되면 멈춘다
	if (veloLength < 5.f)
	{
		m_Velocity = Vec3(0.f, 0.f, 0.f);
	}
	else
	{
		// 감속 적용
		m_Velocity += dragAccel * DT;
		// 회전시킨다
		vRot.z += m_Speed / 10.f * m_Dir.x * DT;
		vRot.x += m_Speed / 10.f * m_Dir.z * DT;
	}


	// 위치값을 갱신한다
	vPos = vPos + m_Velocity * DT;

	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeRotation(vRot);
}

void ThrowableController::MakeBounce(Vec3 _Normal, float _elastic, float _friction)
{
	// 현재 속도
	Vec3 curVelo = m_Velocity;

	// 노말 성분 (물체의 속도가 얼만큼의 크기로 지면으로 향하고 있었는지 )
	float dotProduct = curVelo.Dot(_Normal);

	// 만약 물체가 지면을 향해 충돌했다면
	if (dotProduct < 0)
	{
		// 반사 공식 계산
		Vec3 reflectVelo = curVelo - (1.f + _elastic) * dotProduct * _Normal;

		// 마찰 계수
		reflectVelo *= _friction;

		// 속도 걩신
		m_Velocity = reflectVelo;
		reflectVelo.Normalize();
		m_Dir = reflectVelo;
	}
}







void ThrowableController::BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	if (_Collider->IsTrigger())
	{
		return;
	}
	else
	{
		Vec3 vHitNormal = _Collider->GetHitNormal();

		MakeBounce(vHitNormal, 1.f, 0.7f);
	}
}

void ThrowableController::Overlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}

void ThrowableController::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}





void ThrowableController::BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider)
{
	if (_Collider->IsTrigger())
	{
		return;
	}
	else
	{
		m_bGround = true;

		// 지형의 노말 벡터를 얻음
		Vec3 vPos = Transform()->GetRelativePos();
		Vec3 LandNormal = _OtherCollider->GetWorldPosLandNormal(vPos);

		Transform()->SetRelativePos(vPos);

		// 노말이 유효하면 사용
		if (LandNormal.Length() > 0.001f)
		{
			// Bounce 
			MakeBounce(LandNormal, 1.f, 0.7f);
		}
	}
}

void ThrowableController::Overlap(CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider)
{
}

void ThrowableController::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider)
{
	if (_Collider->IsTrigger())
	{
		return;
	}
	else
	{
		m_bGround = false;
	}
	
}
