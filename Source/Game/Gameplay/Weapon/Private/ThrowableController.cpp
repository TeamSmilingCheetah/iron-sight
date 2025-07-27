#include "pch.h"
#include "Game/Gameplay/Weapon/Public/ThrowableController.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

#include "Game/Gameplay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Inventory/Public/Item.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Weapon/public/BombController.h"


ThrowableController::ThrowableController()
	: WeaponController(SCRIPT_TYPE::THROWABLESCRIPT)
	, m_BombSoundIdx(-1)
	, m_SmokeSoundIdx(-1)
	, m_PinSoundIdx(-1)
	, m_ThrowSoundIdx(-1)
	, m_BounceSoundIdx_1(-1)
	, m_BounceSoundIdx_2(-1)
	, m_Velocity()
	, m_Dir()
	, m_Mass(0.f)
	, m_Speed(8000.f)
	, m_GravityAccel(1000.f)
	, m_CurClipAccTime(0.f)
	, m_TriggeredTime(6.f)
	, m_ThrowAngle(0.f)
	, m_bGround(false)
	, m_bCanThrow(false)
	, m_bThrow(false)
	, m_bTrigger(false)
	, m_bUseFirstBounceSound(true)
	, m_ThownOwner(nullptr)
	, m_Player(nullptr)
	, m_PlayerScript(nullptr)
	, m_AfterThrowAccTime(0.f)

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
	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));

	// Sound
	m_BombSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\grenade_bomb.mp3");
	m_SmokeSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\grenade_smoke.mp3");
	m_PinSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\grenade_pin.mp3");
	m_ThrowSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\grenade_throw.mp3");
	m_BounceSound_1 = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\grenade_bounce1.mp3");
	m_BounceSound_2 = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\grenade_bounce2.mp3");

	WeaponController::Begin();
}

void ThrowableController::Tick()
{
	PlayerCharacter* pPlayerScript = nullptr;

	// 소유주가 있다면 위치를 0으로 초기화
	if (m_EquippedOwner != nullptr)
	{
		Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
		pPlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_EquippedOwner, SCRIPT_TYPE::PLAYERSCRIPT));
		m_ThownOwner = m_EquippedOwner;
	}
	// 소유주가 없다면 return
	else if(m_EquippedOwner == nullptr && m_bThrow == false)
	{
		return;
	}

	if(m_EquippedOwner)
		AdjustFPSPos();

	// 투척 준비 상태 진입
	if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::TAP)
	{
		m_bCanThrow = true;
		pPlayerScript->SetThrow(true);
		ClearKey();

		// 상태
		m_PlayerScript->SetActionState(ACTION_STATE::GRENADE_PREPARE);
	}


	// 투척 준비
	if (m_CurKey == KEY::RBTN && m_CurKeyState == KEY_STATE::TAP)
	{
		m_bCanThrow = true;
		pPlayerScript->SetThrow(true);
		ClearKey();

		// 상태
		m_PlayerScript->SetActionState(ACTION_STATE::GRENADE_PREPARE);
	}

	// 투척 상태 진입 가능한 지 판정
	if (m_EquippedOwner && m_bCanThrow)
	{
		bool readyToThrow = m_EquippedOwner->Animator3D()->GetCurClip()->GetFrameLength() - 1 == m_EquippedOwner->Animator3D()->GetCurFrameIdx();

		// 준비 자세가 된 경우
		if (readyToThrow)
		{
			if (m_CurKey == KEY::LBTN &&
				(m_CurKeyState == KEY_STATE::RELEASED || m_CurKeyState == KEY_STATE::NONE))
			{
				// Player의 위치와 방향 정보
				Vec3 vPlayerPos = m_EquippedOwner->Transform()->GetRelativePos();

				// 부모를 없는 독립 개체로 바꿔준다.
				AddChild(nullptr, GetOwner());
				// TEST: 5번 (player object layer)로 변경
				ChangeLayer(GetOwner(), 5);

				// 현재 Player 위치에 무기를 다시 생성시킨다.
				Vec3 vSpanwPos = vPlayerPos;
				vSpanwPos.y += 800.f;
				GetOwner()->Transform()->SetRelativePos(vSpanwPos);

				// Trigger를 꺼서 충돌을 진행할 수 있게 해준다.
				GetOwner()->Collider3D()->SetTrigger(false);

				// 상태
				m_PlayerScript->SetActionState(ACTION_STATE::GRENADE_THROW_HIGH);

				// 애니메이션 다 끝날 때까지 상태 유지
				m_AfterThrowAccTime = 0.f;
				m_bAfterThrow = true;

				m_ThrowAngle = 0.2f;
				m_Speed = 8000.f;
				m_bThrow = true;

				// 던지는 사운드 재생 (중복 x)
				m_ThrowSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_ThrowSound, m_Player->Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, false, false, m_ThrowSoundIdx);
				ClearKey();
			}

			// 투척 한다.
			else if (m_CurKey == KEY::RBTN &&
				(m_CurKeyState == KEY_STATE::RELEASED || m_CurKeyState == KEY_STATE::NONE))
			{
				// Player의 위치와 방향 정보
				Vec3 vPlayerPos = m_EquippedOwner->Transform()->GetRelativePos();

				// 부모를 없는 독립 개체로 바꿔준다.
				AddChild(nullptr, GetOwner());
				/// TEST: 5번 (player object layer)로 변경
				ChangeLayer(GetOwner(), 5);

				// 현재 Player 위치에 무기를 다시 생성시킨다.
				Vec3 vSpanwPos = vPlayerPos;
				vSpanwPos.y += 800.f;
				GetOwner()->Transform()->SetRelativePos(vSpanwPos);

				// Trigger를 꺼서 충돌을 진행할 수 있게 해준다.
				GetOwner()->Collider3D()->SetTrigger(false);

				// 상태
				m_PlayerScript->SetActionState(ACTION_STATE::GRENADE_THROW_LOW);

				// 애니메이션 다 끝날 때까지 상태 유지
				m_AfterThrowAccTime = 0.f;
				m_bAfterThrow = true;

				m_ThrowAngle = 1.f;
				m_Speed = 4000.f;
				m_bThrow = true;

				// 던지는 사운드 재생 (중복 x)
				m_ThrowSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_ThrowSound, m_Player->Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, false, false, m_ThrowSoundIdx);
				ClearKey();
			}
		}
	}





	//
	if (m_CurKey == KEY::R && m_CurKeyState == KEY_STATE::TAP)
	{
		// 핀소리
		m_PinSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_PinSound, m_Player->Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, true, true, -1);

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

	if (m_bAfterThrow)
	{
		if (m_AfterThrowAccTime < m_ActionEndTime)
		{
			m_AfterThrowAccTime += DT;
		}
		else
		{
			m_PlayerScript->SetActionState(ACTION_STATE::NONE);
			m_bAfterThrow = false;
		}
	}
}


void ThrowableController::Triggered()
{
	m_CurClipAccTime += DT;

	// 시간이 지나면 작동한다.
	if (m_TriggeredTime < m_CurClipAccTime)
	{
		if (m_EquippedOwner != nullptr)
		{
			m_PlayerScript->SetThrowBoom(true);
		}

		if (GetOwner()->GetName() == L"Smoke Grenade")
		{
			if (!GetOwner()->IsDead())
			{
				// 연막 소리
				m_SmokeSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_SmokeSound, Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, true, true, -1);

				Ptr<CPrefab> SmokeParticelPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\SmokeParticle.pref", L"Prefab\\SmokeParticle.pref");
				Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
				vPos.y += 800.f;

				Instantiate(SmokeParticelPrefab, vPos, 0);
			}
		}
		if (GetOwner()->GetName() == L"Grenade")
		{
			if (!GetOwner()->IsDead())
			{
				// 폭발 소리
				m_BombSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_BombSound, Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, true, true, -1);

				Ptr<CPrefab> GrenadeBombPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\GrenadeBomb.pref", L"Prefab\\GrenadeBomb.pref");
				BombController* pBombScript = static_cast<BombController*>(GetScriptWithType(GrenadeBombPrefab->GetProtoObject(), SCRIPT_TYPE::BOMBSCRIPT));
				pBombScript->SetWeaponOwner(m_ThownOwner);
				Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
				//vPos.y += 800.f;

				// ** 폭발 생성 레이어 고민 필요 (피아 구분없이 영향을 줘야 함)
				Instantiate(GrenadeBombPrefab, vPos, 0);
			}
		}

		// 플레이어 슬롯에 장착된 상태로 폭발하는 경우
		if (m_Player && m_EquippedOwner && !GetOwner()->IsDead())
		{
			// 폭발 소리
			m_BombSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_BombSound, m_Player->Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, true, true, -1);

			ItemScript* pItem = static_cast<ItemScript*>(GetScriptWithType(GetOwner(), SCRIPT_TYPE::ITEMSCRIPT));
			static_cast<InventoryController*>(GetScriptWithType(m_Player, SCRIPT_TYPE::INVENTORYSCRIPT))->UseItem(pItem->GetItemType(), 1);
			m_PlayerScript->SetThrow(false);
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
	SetEquippedOwner(nullptr);

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

	// 노말 성분 (물체의 속도가 얼만큼의 크기로 목표로 향하고 있었는지 )
	float dotProduct = curVelo.Dot(_Normal);

	// 만약 물체가 목표를 향해 충돌했다면
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
			// 마무리 튕기는 사운드 재생 (중복 x)
			if (m_bUseFirstBounceSound)
			{
				m_BounceSoundIdx_1 = CSoundMgr::GetInst()->Play3DSound(m_BounceSound_1, Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, false, false, m_BounceSoundIdx_1);
			}
			else
			{
				m_BounceSoundIdx_2 = CSoundMgr::GetInst()->Play3DSound(m_BounceSound_2, Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, false, false, m_BounceSoundIdx_2);
			}
			m_bUseFirstBounceSound = !m_bUseFirstBounceSound;

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
