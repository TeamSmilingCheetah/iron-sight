#include "pch.h"
#include "Game/Gameplay/Character/Public/EnemyController.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Game/Gameplay/UI/Public/KillinfoUIScript.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"
#include "Engine/Runtime/Public/Component/Rendering/LandScape.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Component/Physics/MeshCollider.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

EnemyController::EnemyController(SCRIPT_TYPE _Type)
	: CScript(_Type)
	  , m_Force(0.f)
	  , m_Velocity(0.f)
	  , m_GravidyVelocity(0.f)
	  , m_Mass(3.f)
	  , m_Friction(100.f)
	  , m_MaxSpeed(10.f)
	  , m_GravityAccel(10.f)
	  , m_GravityMaxSpeed(30.f)
	  , m_IsGround(true)
	  , m_HP(100.f)
	  , m_State(Enemy_State::None)
	  , m_PrevState(Enemy_State::None)
	  , m_KillinfoScript(nullptr)
	  , m_Player(nullptr)
	  , m_PlayerScript(nullptr)
{
}

EnemyController::~EnemyController()
{
}

void EnemyController::Begin()
{
	for (int i = 0; i < static_cast<int>(KEY::END); ++i)
	{
		m_vecAIKey.push_back(AItKeyInfo{KEY_STATE::NONE, false});
	}

	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));

	CGameObject* killinfoUI = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Killinfo_UI");
	m_KillinfoScript = static_cast<KillinfoUIScript*>(GetScriptWithType(killinfoUI, SCRIPT_TYPE::KILLINFOUI));
}

void EnemyController::Tick()
{
	// 키 입력 확인
	KeyTick();

	// 키 처리
	KeyInputProcessing();

	// 이동
	UpdatePosition();

	// 회전
	UpdateRotation();
}

void EnemyController::DamageCalcul(CGameObject* _AtkObject, CGameObject* _Weapon, float _Damage)
{
	m_HP -= _Damage;

	// 0보다 낮으면 사망
	if (m_HP <= 0)
	{
		m_HP = 0;
		m_State = Enemy_State::Death;

		// Player가 죽인 경우
		if (_AtkObject->GetName() == L"Player")
		{
			m_PlayerScript->PlusKillCount();
			m_KillinfoScript->SetKillInfo(_AtkObject->GetName(), GetOwner()->GetName(), _Weapon->GetName());
			m_KillinfoScript->OnEvent();
		}
	}
}


void EnemyController::UpdatePosition()
{
	// 힘을 0 으로 초기화
	m_Force = Vec3(0.f, 0.f, 0.f);

	Vec3 vPos = Transform()->GetRelativePos();

	// 이동할 방향 연산
	UpdateMove();

	// 중력 연산
	UpdateGravity();

	// 충돌 연산
	UpdateCollision();

	vPos += m_Velocity;
	Transform()->SetRelativePos(vPos);

	// 충돌벡터 초기화
	m_vecCollisionNormal.clear();
}

void EnemyController::UpdateRotation()
{
	// 이동 방향이 있을 경우 해당 방향으로 부드럽게 회전
	if (m_InputMoveDir.Length() > 0.001f)
	{
		// 목표 방향 벡터 (y값은 0으로 설정하여 수평 회전만 적용)
		Vec3 targetDir = m_InputMoveDir;
		targetDir.y = 0.0f;
		targetDir.Normalize();

		// 현재 전방 벡터 (정면 방향)
		Vec3 currentFront = Transform()->GetWorldDir(DIR_TYPE::FRONT);
		currentFront.y = 0.0f;
		currentFront.Normalize();

		// 외적 계산하여 회전 방향 결정
		Vec3 cross = currentFront.Cross(targetDir);

		// 회전 속도
		float rotateSpeed = 10.0f; // 초당 10도 회전

		// 외적의 y 성분에 따라 회전 방향 결정
		float rotateAmount;
		if (cross.y > 0.0f)
		{
			rotateAmount = rotateSpeed * DT;
		}
		else
		{
			rotateAmount = -rotateSpeed * DT;
		}

		// 현재 회전값 가져와서 Y축만 업데이트
		Vec3 currentRotation = Transform()->GetRelativeRotation();
		currentRotation.y += rotateAmount;
		Transform()->SetRelativeRotation(currentRotation);
	}
}

void EnemyController::UpdateMove()
{
	// 이전 틱의 이동방향저장
	Vec3 vPrevVelocityDir = m_Velocity;
	if (vPrevVelocityDir.Length() > 0.001f)
	{
		vPrevVelocityDir.Normalize();
	}

	// 힘의 방향과 량
	float ForceScar = m_InputMoveForce;
	Vec3 vInputDir = m_InputMoveDir;


	// 입력이 있는 경우
	if (vInputDir.Length() > 0.f)
	{
		float directionDot = vInputDir.Dot(vPrevVelocityDir);

		// 방향 전환이 큰 경우(90도 이상) 속도를 빠르게 줄이고 해당방향 힘을 강하게 증가
		if (directionDot < 0.f && m_Velocity.Length() > 1.0f)
		{
			// 방향 전환 시 속도 감소
			float reductionFactor = 0.5f;
			m_Velocity *= reductionFactor;

			// 방향 전환 시 추가 힘 적용
			ForceScar *= 1.5f;
		}

		// 힘벡터 완성
		m_Force = vInputDir * ForceScar;

		// 방향키입력에 따라 가속도백터 연산
		m_Accel = m_Force / m_Mass;

		// 속도 벡터 연산
		m_Velocity += m_Accel * DT;

		// 최대속도 확인(땅위에 있을 경우에만 판단)
		if (m_IsGround && m_MaxSpeed < m_Velocity.Length())
		{
			m_Velocity.Normalize();
			m_Velocity *= m_MaxSpeed;
		}
	}
	// 이동이 없다면 마찰계수에 따라 감속
	else if (m_IsGround)
	{
		// 속도의 반대방향으로 마찰계수*질량을 곱합
		Vec3 vFriction = -m_Velocity;
		vFriction.Normalize();
		vFriction *= m_Friction * m_Mass * DT;

		// 마찰력이 더 높다면 속도0
		if (m_Velocity.Length() < vFriction.Length())
		{
			m_Velocity = Vec3(0.f, 0.f, 0.f);
		}
		else
		{
			m_Velocity += vFriction;
		}
	}
}

void EnemyController::UpdateGravity()
{
	// 땅 위에있나 판단
	m_IsGround = false;
	for (int i = 0; i < m_vecCollisionNormal.size(); ++i)
	{
		// 노말의 y성분이 0.3 이상이면 지면으로 판단 (약 60도)
		if (m_vecCollisionNormal[i].y > 0.3f)
		{
			m_IsGround = true;
			break;
		}
	}

	Vec3 gravityDir = Vec3(0.f, -1.f, 0.f);
	if (!m_IsGround)
	{
		// 중력가속도 추가하여 y축 아래로 중력 속도 변화
		m_GravidyVelocity += (gravityDir * m_GravityAccel * m_Mass) * DT;

		// 최대 중력속도 제한
		if (m_GravidyVelocity.y < -m_GravityMaxSpeed)
			m_GravidyVelocity.y = -m_GravityMaxSpeed;
	}
	else
	{
		// 땅위이며 아래로 내려가는것이 아니라면 속도 0으로 설정
		if (m_GravidyVelocity.y < 0.f)
		{
			m_GravidyVelocity = Vec3(0.f, 0.f, 0.f);
		}
	}

	m_Velocity += m_GravidyVelocity;
}

void EnemyController::UpdateCollision()
{
	// 충돌 처리
	for (int i = 0; i < m_vecCollisionNormal.size(); ++i)
	{
		// 속도벡터가 충돌노말벡터로의 투영길이확인
		float dotProduct = m_Velocity.Dot(m_vecCollisionNormal[i]);
		// 직각이거나 예각인 상태는 걸러냄 충돌노말벡터로 향하는게 아니니 걸러냄
		if (dotProduct < 0.f)
		{
			// 충돌 방향으로의 속도 성분 제거
			m_Velocity -= m_vecCollisionNormal[i] * dotProduct;
		}
	}
}


void EnemyController::AttachItem(CGameObject* _Item, CGameObject* _BoneObject, Vec3 _RelativePos, Vec3 _RelativeRot)
{
	AddChild(_BoneObject, _Item);
	_Item->Transform()->SetRelativePos(_RelativePos);
	_Item->Transform()->SetRelativeRotation(_RelativeRot);
}

void EnemyController::DetachItem(CGameObject* _Item)
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRotation();

	// 아이템 레이어로 변경
	assert(CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(6)->GetName() == L"Item");
	ChangeLayer(_Item, 6);
	SetObjectActive(_Item, true);
	AttachItem(_Item, nullptr, vPos, vRot);
}

void EnemyController::KeyPush(KEY _Key)
{
	// 키 배열에서 키입력 처리
	AItKeyInfo* AiKey = &m_vecAIKey[(int)_Key];
	AiKey->Presssed = true;
}

void EnemyController::KeyTick()
{
	// 키가 눌려있는지 확인
	for (size_t i = 0; i < m_vecAIKey.size(); ++i)
	{
		if (m_vecAIKey[i].Presssed)
		{
			// 키가 눌려있고, 이전에는 눌려있지 않았다.
			if (false == m_vecAIKey[i].PrevPressed)
			{
				m_vecAIKey[i].State = KEY_STATE::TAP;
			}

			// 키가 눌려있고, 이전에도 눌려있었다.
			else
			{
				m_vecAIKey[i].State = KEY_STATE::PRESSED;
			}

			// 다음 검사를 위해 키입력 상황 전환
			m_vecAIKey[i].PrevPressed = true;
			m_vecAIKey[i].Presssed = false;
		}

		// 키가 눌려있지 않다면
		else
		{
			// 이전에는 눌려있었다.
			if (m_vecAIKey[i].PrevPressed)
			{
				m_vecAIKey[i].State = KEY_STATE::RELEASED;
			}
			else
			{
				m_vecAIKey[i].State = KEY_STATE::NONE;
			}

			m_vecAIKey[i].PrevPressed = false;
		}
	}
}

void EnemyController::BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::Collider3D &&
		InOtherCollider->GetColliderType() == EColliderType::Collider3D)
	{
		FBoxCollider* Collider = static_cast<FBoxCollider*>(InCollider);
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		// 트리거용 충돌체면 해당 코드 사용 x
		if (OtherCollider->IsTrigger())
		{
			return;
		}
		else
		{
			Vec3 pPos = Transform()->GetRelativePos();

			Vec3 hitNormal = Collider->GetCollisionNormal();
			hitNormal.Normalize();

			//Vec3 hitPoint = _Collider->GetHitPoint();
			//Transform()->SetRelativePos(hitPoint);

			// 노말이 유효하면 사용
			if (hitNormal.Length() > 0.001f)
			{
				// 충돌 노말 추가
				m_vecCollisionNormal.push_back(hitNormal);
			}
			else
			{
				Vec3 myPos = Transform()->GetRelativePos();
				Vec3 otherPos = OtherObject->Transform()->GetRelativePos();
				Vec3 normal = myPos - otherPos;
				normal.Normalize();

				// 충돌 벡터 추가
				m_vecCollisionNormal.push_back(normal);
			}
		}
	}
}

void EnemyController::Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::Collider3D &&
		InOtherCollider->GetColliderType() == EColliderType::Collider3D)
	{
		FBoxCollider* Collider = static_cast<FBoxCollider*>(InCollider);
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		// 트리거용 충돌체면 해당 코드 사용 x
		if (OtherCollider->IsTrigger())
		{
			return;
		}
		else
		{
			Vec3 hitNormal = Collider->GetCollisionNormal();
			// 노말이 유효하면 사용
			if (hitNormal.Length() > 0.001f)
			{
				// 충돌 노말 추가
				m_vecCollisionNormal.push_back(hitNormal);
			}
			else
			{
				Vec3 myPos = Transform()->GetRelativePos();
				Vec3 otherPos = OtherObject->Transform()->GetRelativePos();
				Vec3 normal = myPos - otherPos;
				normal.Normalize();
				// 노말이 유효하면 사용
				if (hitNormal.Length() > 0.001f)
				{
					// 충돌 노말 추가
					m_vecCollisionNormal.push_back(hitNormal);
				}
				else
				{
					Vec3 myPos = Transform()->GetRelativePos();
					Vec3 otherPos = OtherObject->Transform()->GetRelativePos();
					Vec3 normal = myPos - otherPos;
					normal.Normalize();

					// 충돌 벡터 추가
					m_vecCollisionNormal.push_back(normal);
				}
			}
		}
	}

	if (InCollider->GetColliderType() == EColliderType::Collider3D &&
		InOtherCollider->GetColliderType() == EColliderType::MeshCollider)
	{
		FMeshCollider* OtherCollider = static_cast<FMeshCollider*>(InOtherCollider);

		Vec3 CollisionNormal = OtherCollider->GetCollisionNormal();
		float PenetrationDepth = OtherCollider->GetPenetrationDepth();

		// Valid Check
		if (CollisionNormal.Length() > 0.001f)
		{
			// Add Normal
			m_vecCollisionNormal.push_back(CollisionNormal);

			// Position Correction
			// 보정 제한은 임의의 값 Setting
			if (PenetrationDepth > 0.0f && PenetrationDepth < 10.0f)
			{
				// Speed Decay By Direction
				float VelocityDotNormal = m_Velocity.Dot(CollisionNormal);
				if (VelocityDotNormal < 0.0f)
				{
					Vec3 ReflectedVelocity = m_Velocity - 2.0f * VelocityDotNormal * CollisionNormal;
					m_Velocity = ReflectedVelocity * 0.8f;

					// Additional Position Correction
					Vec3 SmallCorrection = CollisionNormal * 0.01f;
					Transform()->SetRelativePos(Transform()->GetRelativePos() + SmallCorrection);
				}
			}
		}
	}
}

void EnemyController::EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
}

// void EnemyController::BeginOverlap(FCollider3D* _Collider, CGameObject* _OtherObject, FLandScape* _OtherCollider)
// {
// 	Vec3 pPos = Transform()->GetRelativePos();
//
// 	// 지형의 노말 벡터를 얻음
// 	Vec3 LandNormal = _OtherCollider->GetWorldPosLandNormal(pPos);
//
// 	Transform()->SetRelativePos(pPos);
//
// 	// 노말이 유효하면 사용
// 	if (LandNormal.Length() > 0.001f)
// 	{
// 		// 충돌 노말 추가
// 		m_vecCollisionNormal.push_back(LandNormal);
// 	}
// }
//
// void EnemyController::Overlap(FCollider3D* _Collider, CGameObject* _OtherObject, FLandScape* _OtherCollider)
// {
// 	Vec3 pPos = Transform()->GetRelativePos();
//
// 	// 지형의 노말 벡터를 얻음
// 	Vec3 LandNormal = _OtherCollider->GetWorldPosLandNormal(pPos);
//
// 	// 노말이 유효하면 사용
// 	if (LandNormal.Length() > 0.001f)
// 	{
// 		// 충돌 노말 추가
// 		m_vecCollisionNormal.push_back(LandNormal);
// 	}
// }
//
// void EnemyController::EndOverlap(FCollider3D* _Collider, CGameObject* _OtherObject, FLandScape* _OtherCollider)
// {
// }


void EnemyController::SaveComponent(FILE* _File)
{
}

void EnemyController::LoadComponent(FILE* _File)
{
}
