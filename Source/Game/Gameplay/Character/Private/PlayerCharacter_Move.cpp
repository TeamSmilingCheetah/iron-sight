#include "pch.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/Runtime/Public/Component/Physics/CMeshCollider.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

#include "Game/Gameplay/Character/Public/CameraController.h"

void PlayerCharacter::PlayerMove()
{
	// 이동할 방향 연산
	UpdateMove();

	// 중력 연산
	UpdateGravity();

	// 충돌 연산
	UpdateCollision();

	// 위치 업데이트
	Vec3 vPos = Transform()->GetRelativePos();
	vPos += m_Velocity * 100 * DT;

	Transform()->SetRelativePos(vPos);

	// 애니메이션
	AnimationControl();

	// 초기화
	InitMove();
}

void PlayerCharacter::InitMove()
{
	// 힘을 0 으로 초기화
	m_Force = Vec3(0.f, 0.f, 0.f);

	// 충돌벡터 초기화
	m_vecCollisionNormal.clear();
}

void PlayerCharacter::UpdateMove()
{
	// 방향키입력 확인
	Vec3 vRot = Transform()->GetRelativeRotation();
	//float radian = vRot.y * XM_PI / 180.f;

	//Vec3 vFowardDir = { -sinf(radian), 0.f, -cosf(radian) };
	Vec3 vFowardDir = Transform()->GetWorldDir(DIR_TYPE::FRONT);
	//Vec3 vRightDir = { -cosf(radian), 0.f, sinf(radian) };
	Vec3 vRightDir = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vInputDir = {0.f, 0.f, 0.f};

	// 이전 틱의 이동방향저장
	Vec3 vPrevVelocityDir = m_Velocity;
	if (vPrevVelocityDir.Length() > 0.001f)
	{
		vPrevVelocityDir.Normalize();
	}

	// 힘의 량
	float ForceScar;

	if (KEY_PRESSED(KEY::LSHIFT) &&
		KEY_PRESSED(KEY::W) &&
		!(KEY_PRESSED(KEY::A)) &&
		!(KEY_PRESSED(KEY::S)) &&
		!(KEY_PRESSED(KEY::D)) &&
		!m_CamScript->GetFlag(ADS) &&
		!m_CamScript->GetFlag(SITTING) &&
		!m_CamScript->GetFlag(LAYING) &&
		!m_bLean &&
		!m_bReloading)
	{
		ForceScar = 80.f;
		m_MaxSpeed = 20.f;
	}
	else if (m_CamScript->GetFlag(SITTING))
	{
		ForceScar = 30.f;
		m_MaxSpeed = 5.f;
	}
	else if (m_CamScript->GetFlag(LAYING))
	{
		ForceScar = 20.f;
		m_MaxSpeed = 2.f;
	}
	else
	{
		ForceScar = 50.f;
		m_MaxSpeed = 10.f;
	}

	// 해당하는 방향으로 벡터를 추가한다.
	if (KEY_PRESSED(KEY::W))
	{
		vInputDir += -vFowardDir;
	}
	if (KEY_PRESSED(KEY::A))
	{
		vInputDir += vRightDir;
	}
	if (KEY_PRESSED(KEY::S))
	{
		vInputDir += vFowardDir;
	}
	if (KEY_PRESSED(KEY::D))
	{
		vInputDir += -vRightDir;
	}

	// 부스트에 의한 이동속도 보정
	m_MaxSpeed *= m_BoostSpeed;

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

	// 방향키 입력이 없다면 마칠계수에 따라 감속(땅에있다는 조건추가)
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

void PlayerCharacter::UpdateGravity()
{
	// 땅 위에 있나 판단
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
		m_GravityVelocity += (gravityDir * m_GravityAccel * m_Mass) * DT;

		// 최대 중력속도 제한
		m_GravityVelocity.y = max(m_GravityVelocity.y, -m_GravityMaxSpeed);
	}
	else
	{
		// 땅위이며 아래로 내려가는것이 아니라면 속도 0으로 설정
		if (m_GravityVelocity.y < 0.f)
		{
			m_GravityVelocity = Vec3(0.f, 0.f, 0.f);

			// 상태
			if (m_ActionState == ACTION_STATE::JUMP)
			{
				SetActionState(ACTION_STATE::NONE);
			}
		}
	}

	// 점프 기능
	if (m_IsGround && KEY_TAP(KEY::SPACE))
	{
		m_GravityVelocity += Vec3(0.f, 1.f, 0.f) * m_JumpPower;

		// 상태
		SetActionState(ACTION_STATE::JUMP);
	}

	m_Velocity += m_GravityVelocity;
}

void PlayerCharacter::UpdateCollision()
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

void PlayerCharacter::AnimationControl()
{
	float delay = 0.f;
	wstring clipName = L"";

	switch (m_ActionState)
	{
	case ACTION_STATE::JUMP:
	{
		delay = 0.02f;
		clipName = L"Animation\\Armature_rifle jump.anim";
	}
		break;
	case ACTION_STATE::GUN_FIRE:
	{
		delay = 0.02f;
		switch (m_MotionState)
		{
		case MOTION_STATE::STAND:
			clipName = L"Animation\\Armature_firing rifle.anim";
			break;
		case MOTION_STATE::CROUCH: // TODO: Crouch Animation
			clipName = L"Animation\\Armature_firing rifle.anim";
			break;
		case MOTION_STATE::PRONE:
			clipName = L"Animation\\Armature_prone firing rifle.anim";
			break;
		}
	}
		break;
	case ACTION_STATE::GUN_RELOAD:
	{
		delay = 0.4f;
		switch (m_MotionState)
		{
		case MOTION_STATE::STAND:
			clipName = L"Animation\\Armature_reloading.anim";
			break;
		case MOTION_STATE::CROUCH: // TODO: Crouch Animation
			clipName = L"Animation\\Armature_reloading.anim";
			break;
		case MOTION_STATE::PRONE:
			clipName = L"Animation\\Armature_prone reloading.anim";
			break;
		}
	}
		break;
	case ACTION_STATE::GRENADE:
	{
		delay = 1.f;
		switch (m_MotionState)
		{
		case MOTION_STATE::STAND:
			clipName = L"Animation\\Armature_toss grenade.anim";
			break;
		case MOTION_STATE::CROUCH:	// TODO: Crouch Animation
			clipName = L"Animation\\Armature_toss grenade.anim";
			break;
		case MOTION_STATE::PRONE:
			clipName = L"Animation\\Armature_prone toss grenade.anim";
			break;
		}
	}
		break;
	case ACTION_STATE::BANDAGE:
		break;
	case ACTION_STATE::MED_KIT:
		break;
	case ACTION_STATE::FIRST_AID_KIT:
		break;
	case ACTION_STATE::ENERGY_DRINK:
		break;
	case ACTION_STATE::PAIN_KILLER:
		break;
	case ACTION_STATE::ADRENALINE_SYRINGE:
		break;
	case ACTION_STATE::DEAD:
	{
		delay = 1.f;
		switch (m_MotionState)
		{
		case MOTION_STATE::STAND:
			clipName = L"Animation\\Armature_death from the front.anim";
			break;
		case MOTION_STATE::CROUCH:	// TODO: Crouch Animation
			clipName = L"Animation\\Armature_death from the front.anim";
			break;
		case MOTION_STATE::PRONE:
			clipName = L"Animation\\Armature_prone death.anim";
			break;
		}
	}
		break;
	case ACTION_STATE::NONE:
	{
		delay = 0.07f;

		if (KEY_PRESSED(KEY::W))
		{
			if (m_MotionState == MOTION_STATE::CROUCH)
				clipName = L"Animation\\Armature_walk crouching forward.anim";
			else if (m_MotionState == MOTION_STATE::PRONE)
				clipName = L"Animation\\Armature_prone forward.anim";
			else if (KEY_PRESSED(KEY::LSHIFT))
				clipName = L"Animation\\Armature_run forward.anim";
			else
				clipName = L"Animation\\Armature_walk forward.anim";
		}
		else if (KEY_PRESSED(KEY::A))
		{
			if (m_MotionState == MOTION_STATE::CROUCH)
				clipName = L"Animation\\Armature_walk crouching left.anim";
			else if (m_MotionState == MOTION_STATE::PRONE)
				clipName = L"Animation\\Armature_prone left.anim";
			else if (KEY_PRESSED(KEY::LSHIFT))
				clipName = L"Animation\\Armature_run left.anim";
			else
				clipName = L"Animation\\Armature_walk left.anim";
		}
		else if (KEY_PRESSED(KEY::S))
		{
			if (m_MotionState == MOTION_STATE::CROUCH)
				clipName = L"Animation\\Armature_walk crouching backward.anim";
			else if (m_MotionState == MOTION_STATE::PRONE)
				clipName = L"Animation\\Armature_prone backward.anim";
			else if (KEY_PRESSED(KEY::LSHIFT))
				clipName = L"Animation\\Armature_run backward.anim";
			else
				clipName = L"Animation\\Armature_walk backward.anim";
		}
		else if (KEY_PRESSED(KEY::D))
		{
			if (m_MotionState == MOTION_STATE::CROUCH)
				clipName = L"Animation\\Armature_walk crouching right.anim";
			else if (m_MotionState == MOTION_STATE::PRONE)
				clipName = L"Animation\\Armature_prone right.anim";
			else if (KEY_PRESSED(KEY::LSHIFT))
				clipName = L"Animation\\Armature_run right.anim";
			else
				clipName = L"Animation\\Armature_walk right.anim";
		}
		else
		{
			if (m_MotionState == MOTION_STATE::CROUCH)
				clipName = L"Animation\\Armature_idle crouching.anim";
			else if (m_MotionState == MOTION_STATE::PRONE)
				clipName = L"Animation\\Armature_prone idle.anim";
			else
				clipName = L"Animation\\Armature_idle.anim";
		}
	}
		break;
	}


	if (Animator3D()->GetCurClip()->GetKey() != clipName
		&& (Animator3D()->GetNextClip() == nullptr ||  Animator3D()->GetNextClip()->GetKey() != clipName))
	{
		Animator3D()->SetCurClipBlend(clipName, delay);
	}
}


void PlayerCharacter::BeginOverlap(CCollider3D* PCollider, CGameObject* POtherObject, CCollider3D* POtherCollider)
{
	// 트리거용 충돌체면 해당 코드 사용 x
	if (POtherCollider->IsTrigger())
	{
		return;
	}
	else
	{
		Vec3 hitNormal = PCollider->GetHitNormal();
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
			Vec3 otherPos = POtherObject->Transform()->GetRelativePos();
			Vec3 normal = myPos - otherPos;
			normal.Normalize();

			// 충돌 벡터 추가
			m_vecCollisionNormal.push_back(normal);
		}
	}
}

void PlayerCharacter::Overlap(CCollider3D* PCollider, CGameObject* POtherObject, CCollider3D* POtherCollider)
{
	// 트리거용 충돌체면 해당 코드 사용 x
	if (POtherCollider->IsTrigger())
	{
	}
	else
	{
		Vec3 hitNormal = PCollider->GetHitNormal();
		// 노말이 유효하면 사용
		if (hitNormal.Length() > 0.001f)
		{
			// 충돌 노말 추가
			m_vecCollisionNormal.push_back(hitNormal);
		}
		else
		{
			// 노말이 유효하면 사용
			if (hitNormal.Length() > 0.001f)
			{
				// 충돌 노말 추가
				m_vecCollisionNormal.push_back(hitNormal);
			}
			else
			{
				Vec3 myPos = Transform()->GetRelativePos();
				Vec3 otherPos = POtherObject->Transform()->GetRelativePos();
				Vec3 normal = myPos - otherPos;
				normal.Normalize();

				// 충돌 벡터 추가
				m_vecCollisionNormal.push_back(normal);
			}
		}
	}
}

void PlayerCharacter::EndOverlap(CCollider3D* PCollider, CGameObject* POtherObject, CCollider3D* POtherCollider)
{
}


void PlayerCharacter::BeginOverlap(CColliderRay* PRayCollider, CGameObject* POtherObject, CCollider3D* P3DCollider)
{
	m_CollObject = POtherObject;
}

void PlayerCharacter::Overlap(CColliderRay* PRayCollider, CGameObject* POtherObject, CCollider3D* P3DCollider)
{
}

void PlayerCharacter::EndOverlap(CColliderRay* PRayCollider, CGameObject* POtherObject, CCollider3D* P3DCollider)
{
	if (m_CollObject == POtherObject)
		m_CollObject = nullptr;
}

void PlayerCharacter::BeginOverlap(CCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider)
{
	Vec3 pPos = Transform()->GetRelativePos();

	//// 충돌 전 위치로 약간 되돌림
	//Vec3 prevPos = pPos - m_Velocity * DT;

	// 지형의 노말 벡터를 얻음
	Vec3 LandNormal = POtherCollider->GetWorldPosLandNormal(pPos);

	Transform()->SetRelativePos(pPos);

	// 노말이 유효하면 사용
	if (LandNormal.Length() > 0.001f)
	{
		// 충돌 노말 추가
		m_vecCollisionNormal.push_back(LandNormal);

		//// 파고듬 방지
		//float dotProduct = m_Velocity.Dot(LandNormal);
		//if (dotProduct < 0.f)
		//{
		//	// 더 위치 옮겨봄
		//	Vec3 adjustedPos = pPos + LandNormal * 0.05f;
		//	Transform()->SetRelativePos(adjustedPos);
		//}
	}
}

void PlayerCharacter::Overlap(CCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider)
{
	Vec3 pPos = Transform()->GetRelativePos();

	//// 충돌 전 위치로 약간 되돌림
	//Vec3 prevPos = pPos - m_Velocity * DT;

	// 지형의 노말 벡터를 얻음
	Vec3 LandNormal = POtherCollider->GetWorldPosLandNormal(pPos);

	// 노말이 유효하면 사용
	if (LandNormal.Length() > 0.001f)
	{
		// 충돌 노말 추가
		m_vecCollisionNormal.push_back(LandNormal);

		//// 지속적으로 파고듬 방지
		//float dotProduct = m_Velocity.Dot(LandNormal);
		//if (dotProduct < 0.f)
		//{
		//	// 혹시 모르니 더 옳김
		//	Vec3 adjustedPos = pPos + LandNormal * 0.05f;
		//	Transform()->SetRelativePos(adjustedPos);
		//}
	}
}

void PlayerCharacter::EndOverlap(CCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider)
{
}

/**
 * @brief 플레이어 기준, 3D Collider와 Mesh Collider가 충돌했을 때 처리하는 BeginOverlap 함수
 *
 * @param PCollider
 * @param POtherObject
 * @param POtherCollider
 */
void PlayerCharacter::BeginOverlap(CCollider3D* PCollider, CGameObject* POtherObject, CMeshCollider* POtherCollider)
{
	Vec3 CollisionNormal = POtherCollider->GetCollisionNormal();
	float PenetrationDepth = POtherCollider->GetPenetrationDepth();

	// Valid Check
	if (CollisionNormal.Length() > 0.001f)
	{
		// Add Normal
		m_vecCollisionNormal.push_back(CollisionNormal);

		// Position Correction
		// 보정 제한은 임의의 값 Setting
		if (PenetrationDepth > 0.0f && PenetrationDepth < 10.0f)
		{
			Vec3 Correction = CollisionNormal * PenetrationDepth;
			Transform()->SetRelativePos(Transform()->GetRelativePos() + Correction);
		}
		else
		{
			// Set Default Margin
			constexpr float DefaultMargin = 0.05f;
			Vec3 Correction = CollisionNormal * DefaultMargin;
			Transform()->SetRelativePos(Transform()->GetRelativePos() + Correction);
		}
	}
}

/**
 * @brief 플레이어 기준, 3D Collider와 Mesh Collider가 충돌했을 때 처리하는 Overlap 함수
 *
 * @param PCollider
 * @param POtherObject
 * @param POtherCollider
 */
void PlayerCharacter::Overlap(CCollider3D* PCollider, CGameObject* POtherObject, CMeshCollider* POtherCollider)
{
	Vec3 CollisionNormal = POtherCollider->GetCollisionNormal();
	float PenetrationDepth = POtherCollider->GetPenetrationDepth();

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

void PlayerCharacter::EndOverlap(CCollider3D* PCollider, CGameObject* POtherObject, CMeshCollider* POtherCollider)
{
}
