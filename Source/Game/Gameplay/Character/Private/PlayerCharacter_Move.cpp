#include "pch.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"
#include "Engine/Runtime/Public/Component/Physics/MeshCollider.h"
#include "Engine/Runtime/Public/Component/Rendering/Landscape.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/System/Public/Manager/SoundManager.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

#include "Game/System/Public/CGameMgr.h"
#include "Game/Gameplay/Character/Public/CameraController.h"

// Mock function For Landscape Height
static float GetLandscapeHeight(float x, float z)
{
	return -790.f;
}

void PlayerCharacter::ProgressPlayerMove()
{
	// 1. 이동 입력 처리
	UpdateMove();

	// 2. 실시간 지면 판정 + 중력 / 점프 처리
	UpdateGravity();

	// 3. 충돌 처리 (벽면 / 오브젝트 충돌만)
	UpdateCollision();

	// 4. 위치 업데이트 + Landscape 보정
	UpdatePosition();

	// 애니메이션
	// AnimationControl();

	// 5. 다음 틱을 위한 초기화
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

	if (CanRun())
	{
		ForceScar = 80.f;
		m_MaxSpeed = 20.f;
	}
	else if (m_MotionState == MOTION_STATE::CROUCH)
	{
		ForceScar = 30.f;
		m_MaxSpeed = 5.f;
	}
	else if (m_MotionState == MOTION_STATE::PRONE)
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

	// 소리 컨트롤
	Vec2 v2DVelocity = Vec2(m_Velocity.x, m_Velocity.z);

	// 달리기
	if (15.f < v2DVelocity.Length())
	{
		// 걷는 소리 사운드 중지
		FSoundManager::GetInst()->Stop3DSound(m_FootstepSoundIdx);

		// 일정 속도 이상이 되면 소리를 재생시킨다.
		m_RunFootstepSoundIdx = FSoundManager::GetInst()->Play3DSound(m_RunFootstepSound, Transform()->GetRelativePos(),
		                                                          1.f, 10000.f, 1, 0.5f, false, false,
		                                                          m_RunFootstepSoundIdx);

		m_FootStepSoundAccTime += DT;
		// 일정시간이 지나면 소리 위치 업데이트
		if (0.1f <= m_FootStepSoundAccTime)
		{
			FSoundManager::GetInst()->Update3DSoundPosition(m_RunFootstepSoundIdx, Transform()->GetRelativePos());
			m_FootStepSoundAccTime = 0.f;
		}
	}
	// 걷기
	else if (5.f < v2DVelocity.Length())
	{
		// 일정 속도 이상이 되면 소리를 재생시킨다.
		m_FootstepSoundIdx = FSoundManager::GetInst()->Play3DSound(m_FootstepSound, Transform()->GetRelativePos(), 1.f,
		                                                       10000.f, 1, 0.5f, false, false, m_FootstepSoundIdx);


		m_FootStepSoundAccTime += DT;
		// 일정시간이 지나면 소리 위치 업데이트
		if (0.1f <= m_FootStepSoundAccTime)
		{
			FSoundManager::GetInst()->Update3DSoundPosition(m_FootstepSoundIdx, Transform()->GetRelativePos());
			m_FootStepSoundAccTime = 0.f;
		}
	}
	// 소리 중지
	else if (v2DVelocity.Length() < 5.f)
	{
		m_FootStepSoundAccTime = 0.f;
		FSoundManager::GetInst()->Stop3DSound(m_FootstepSoundIdx);
	}
	else if (v2DVelocity.Length() <= 10.f)
	{
		FSoundManager::GetInst()->Stop3DSound(m_RunFootstepSoundIdx);
	}
}

void PlayerCharacter::UpdateGravity()
{
	// 현재 위치에서 실시간 지면 판정
	Vec3 CurrentPosition = Transform()->GetRelativePos();
	float GroundHeight = GetLandscapeHeight(CurrentPosition.x, CurrentPosition.z);
	constexpr float Tolerance = 1.0f;

	// Landscape 기반 지면 판정
	m_IsGround = (CurrentPosition.y <= GroundHeight + Tolerance);

	// 추가적으로 충돌 노말 벡터를 통한 지면 판정도 고려 (계단, 경사면 등)
	if (!m_IsGround)
	{
		for (int i = 0; i < m_vecCollisionNormal.size(); ++i)
		{
			// 노말의 y성분이 0.3 이상이면 지면으로 판단 (약 60도)
			if (m_vecCollisionNormal[i].y > 0.3f)
			{
				m_IsGround = true;
				break;
			}
		}
	}

	Vec3 gravityDir = Vec3(0.f, -1.f, 0.f);
	if (!m_IsGround)
	{
		// 공중에 있을 때만 중력 적용
		m_GravityVelocity += (gravityDir * m_GravityAccel) * DT;

		// 최대 중력속도 제한
		m_GravityVelocity.y = max(m_GravityVelocity.y, -m_GravityMaxSpeed);
	}
	else
	{
		// 지면에 있으면 하향 중력 속도만 초기화
		m_GravityVelocity.y = max(m_GravityVelocity.y, 0.f);

		// TODO(Ssio): State
		// if (m_ActionState == ACTION_STATE::JUMP)
		// {
		// 	SetActionState(ACTION_STATE::NONE);
		// }
	}

	// 점프 기능 (현재 틱의 실시간 지면 판정 사용)
	if (m_IsGround && KEY_TAP(KEY::SPACE))
	{
		m_GravityVelocity += Vec3(0.f, 1.f, 0.f) * m_JumpPower;

		// 상태
		StateMachine()->SetChange(L"Player_Jump");
	}

	// 최종 속도에 중력 속도 합산
	m_Velocity.y = m_GravityVelocity.y;
}

void PlayerCharacter::UpdateCollision()
{
	// 벽면과 오브젝트 충돌 처리 (지면 충돌은 UpdatePosition에서 처리)
	for (int i = 0; i < m_vecCollisionNormal.size(); ++i)
	{
		Vec3 Normal = m_vecCollisionNormal[i];

		// 지면 노말은 여기서 제외 (y 성분이 0.3 이상인 경우)
		if (Normal.y > 0.3f)
		{
			continue;
		}

		// 속도벡터가 충돌노말벡터로의 투영길이확인
		float dotProduct = m_Velocity.Dot(Normal);
		// 충돌 방향으로 향하는 경우에만 속도 조정
		if (dotProduct < 0.f)
		{
			// 충돌 방향으로의 속도 성분 제거 (벽면 슬라이딩)
			m_Velocity -= Normal * dotProduct;
		}
	}
}

bool PlayerCharacter::CanRun()
{
	if (!(KEY_PRESSED(KEY::LSHIFT)))
	{
		return false;
	}
	if (!(KEY_PRESSED(KEY::W)))
	{
		return false;
	}
	if (KEY_PRESSED(KEY::A) || KEY_PRESSED(KEY::S) || KEY_PRESSED(KEY::D))
	{
		return false;
	}
	if (CGameMgr::GetInst()->GetCamScript()->GetFlag(ADS))
	{
		return false;
	}
	if (m_bLean)
	{
		return false;
	}
	if (m_MotionState != MOTION_STATE::STAND)
	{
		return false;
	}

	// 현재 재장전 중이라면 불가능
	if (GetStateEnum() == PLAYER_STATE::Player_Gun_Reload)
	{
		return false;
	}

	return true;
}



//void PlayerCharacter::AnimationControl()
//{
//	float delay = 0.f;
//	wstring clipName = L"";
//
//	switch (m_ActionState)
//	{
//	case ACTION_STATE::JUMP:
//		{
//			delay = 0.02f;
//			clipName = L"Animation\\Armature_rifle_jump.anim";
//		}
//		break;
//	case ACTION_STATE::GUN_FIRE:
//		{
//			delay = 0.02f;
//			switch (m_MotionState)
//			{
//			case MOTION_STATE::STAND:
//				clipName = L"Animation\\Armature_firing_rifle.anim";
//				break;
//			case MOTION_STATE::CROUCH: // TODO: Crouch Animation
//				clipName = L"Animation\\Armature_firing_rifle.anim";
//				break;
//			case MOTION_STATE::PRONE:
//				clipName = L"Animation\\Armature_prone_firing_rifle.anim";
//				break;
//			}
//		}
//		break;
//	case ACTION_STATE::GUN_RELOAD:
//		{
//			delay = 0.4f;
//			switch (m_MotionState)
//			{
//			case MOTION_STATE::STAND:
//				clipName = L"Animation\\Armature_reloading.anim";
//				break;
//			case MOTION_STATE::CROUCH: // TODO: Crouch Animation
//				clipName = L"Animation\\Armature_reloading.anim";
//				break;
//			case MOTION_STATE::PRONE:
//				clipName = L"Animation\\Armature_prone_reloading.anim";
//				break;
//			}
//		}
//		break;
//	case ACTION_STATE::GRENADE_PREPARE:
//		{
//			delay = 1.f;
//			switch (m_MotionState)
//			{
//			case MOTION_STATE::STAND:
//				clipName = L"Animation\\Armature_stand_grenade_prepare.anim";
//				break;
//			case MOTION_STATE::CROUCH: // TODO: Crouch Animation
//				clipName = L"Animation\\Armature_stand_grenade_prepare.anim";
//				break;
//			case MOTION_STATE::PRONE:
//				clipName = L"Animation\\Armature_prone_grenade_prepare.anim";
//				break;
//			}
//		}
//		break;
//	case ACTION_STATE::GRENADE_THROW_HIGH:
//		{
//			delay = 0.07f;
//			switch (m_MotionState)
//			{
//			case MOTION_STATE::STAND:
//				clipName = L"Animation\\Armature_toss_grenade_test.anim";
//				break;
//			case MOTION_STATE::CROUCH: // TODO: Crouch Animation
//				clipName = L"Animation\\Armature_toss_grenade_test.anim";
//				break;
//			case MOTION_STATE::PRONE:
//				clipName = L"Animation\\Armature_prone_toss_grenade_test.anim";
//				break;
//			}
//		}
//		break;
//	case ACTION_STATE::GRENADE_THROW_LOW:
//		{
//			delay = 0.3f;
//			switch (m_MotionState)
//			{
//			case MOTION_STATE::STAND:
//				clipName = L"Animation\\Armature_toss_grenade_low.anim";
//				break;
//			case MOTION_STATE::CROUCH: // TODO: Crouch Animation
//				clipName = L"Animation\\Armature_toss_grenade_low.anim";
//				break;
//			case MOTION_STATE::PRONE:
//				clipName = L"Animation\\Armature_prone_toss_grenade_test.anim";
//				break;
//			}
//		}
//		break;
//	case ACTION_STATE::BANDAGE:
//		break;
//	case ACTION_STATE::MED_KIT:
//		break;
//	case ACTION_STATE::FIRST_AID_KIT:
//		{
//			delay = 0.4f;
//			switch (m_MotionState)
//			{
//			case MOTION_STATE::STAND:
//				clipName = L"Animation\\Armature_stand_first_aid_kit.anim";
//				break;
//			case MOTION_STATE::CROUCH: // TODO: Crouch Animation
//				clipName = L"Animation\\Armature_stand_first_aid_kit.anim";
//				break;
//			case MOTION_STATE::PRONE:
//				clipName = L"Animation\\Armature_prone_first_aid_kit.anim";
//				break;
//			}
//		}
//		break;
//	case ACTION_STATE::ENERGY_DRINK:
//		{
//			delay = 0.4f;
//			switch (m_MotionState)
//			{
//			case MOTION_STATE::STAND:
//				clipName = L"Animation\\Armature_stand_energy_drink.anim";
//				break;
//			case MOTION_STATE::CROUCH: // TODO: Crouch Animation
//				clipName = L"Animation\\Armature_stand_energy_drink.anim";
//				break;
//			case MOTION_STATE::PRONE:
//				clipName = L"Animation\\Armature_prone_energy_drink.anim";
//				break;
//			}
//		}
//		break;
//	case ACTION_STATE::PAIN_KILLER:
//		{
//			delay = 0.4f;
//			switch (m_MotionState)
//			{
//			case MOTION_STATE::STAND:
//				clipName = L"Animation\\Armature_stand_pain_killer.anim";
//				break;
//			case MOTION_STATE::CROUCH: // TODO: Crouch Animation
//				clipName = L"Animation\\Armature_stand_pain_killer.anim";
//				break;
//			case MOTION_STATE::PRONE:
//				clipName = L"Animation\\Armature_prone_pain_killer.anim";
//				break;
//			}
//		}
//		break;
//	case ACTION_STATE::ADRENALINE_SYRINGE:
//		{
//			delay = 0.4f;
//			switch (m_MotionState)
//			{
//			case MOTION_STATE::STAND:
//				clipName = L"Animation\\Armature_reloading.anim";
//				break;
//			case MOTION_STATE::CROUCH: // TODO: Crouch Animation
//				clipName = L"Animation\\Armature_reloading.anim";
//				break;
//			case MOTION_STATE::PRONE:
//				clipName = L"Animation\\Armature_prone_reloading.anim";
//				break;
//			}
//		}
//		break;
//	case ACTION_STATE::DEAD:
//		{
//			delay = 1.f;
//			switch (m_MotionState)
//			{
//			case MOTION_STATE::STAND:
//				clipName = L"Animation\\Armature_death_from_the_front.anim";
//				break;
//			case MOTION_STATE::CROUCH: // TODO: Crouch Animation
//				clipName = L"Animation\\Armature_death_from_the_front.anim";
//				break;
//			case MOTION_STATE::PRONE:
//				clipName = L"Animation\\Armature_prone_death.anim";
//				break;
//			}
//		}
//		break;
//	case ACTION_STATE::NONE:
//		{
//			delay = 0.07f;
//
//			if (KEY_PRESSED(KEY::W))
//			{
//				if (m_MotionState == MOTION_STATE::CROUCH)
//					clipName = L"Animation\\Armature_walk_crouching_forward.anim";
//				else if (m_MotionState == MOTION_STATE::PRONE)
//					clipName = L"Animation\\Armature_prone_forward.anim";
//				else if (KEY_PRESSED(KEY::LSHIFT))
//					clipName = L"Animation\\Armature_run_forward.anim";
//				else
//					clipName = L"Animation\\Armature_walk_forward.anim";
//			}
//			else if (KEY_PRESSED(KEY::A))
//			{
//				if (m_MotionState == MOTION_STATE::CROUCH)
//					clipName = L"Animation\\Armature_walk_crouching_left.anim";
//				else if (m_MotionState == MOTION_STATE::PRONE)
//					clipName = L"Animation\\Armature_prone_left.anim";
//				else if (KEY_PRESSED(KEY::LSHIFT))
//					clipName = L"Animation\\Armature_run_left.anim";
//				else
//					clipName = L"Animation\\Armature_walk_left.anim";
//			}
//			else if (KEY_PRESSED(KEY::S))
//			{
//				if (m_MotionState == MOTION_STATE::CROUCH)
//					clipName = L"Animation\\Armature_walk_crouching_backward.anim";
//				else if (m_MotionState == MOTION_STATE::PRONE)
//					clipName = L"Animation\\Armature_prone_backward.anim";
//				else if (KEY_PRESSED(KEY::LSHIFT))
//					clipName = L"Animation\\Armature_run_backward.anim";
//				else
//					clipName = L"Animation\\Armature_walk_backward.anim";
//			}
//			else if (KEY_PRESSED(KEY::D))
//			{
//				if (m_MotionState == MOTION_STATE::CROUCH)
//					clipName = L"Animation\\Armature_walk_crouching_right.anim";
//				else if (m_MotionState == MOTION_STATE::PRONE)
//					clipName = L"Animation\\Armature_prone_right.anim";
//				else if (KEY_PRESSED(KEY::LSHIFT))
//					clipName = L"Animation\\Armature_run_right.anim";
//				else
//					clipName = L"Animation\\Armature_walk_right.anim";
//			}
//			else
//			{
//				if (m_MotionState == MOTION_STATE::CROUCH)
//					clipName = L"Animation\\Armature_idle_crouching.anim";
//				else if (m_MotionState == MOTION_STATE::PRONE)
//					clipName = L"Animation\\Armature_prone_idle.anim";
//				else
//					clipName = L"Animation\\Armature_idle.anim";
//			}
//		}
//		break;
//	}
//
//
//	if (Animator3D()->GetCurClip()->GetKey() != clipName
//		&& (Animator3D()->GetNextClip() == nullptr || Animator3D()->GetNextClip()->GetKey() != clipName))
//	{
//		Animator3D()->SetCurClipBlend(clipName, delay);
//	}
//}

void PlayerCharacter::UpdatePosition()
{
	// 기본 위치 업데이트
	Vec3 vPos = Transform()->GetRelativePos();
	vPos += m_Velocity * 100 * DT;

	// Landscape 높이 보정
	float groundHeight = GetLandscapeHeight(vPos.x, vPos.z);

	// 지면보다 아래로 내려갔다면 높이 보정
	if (vPos.y < groundHeight)
	{
		vPos.y = groundHeight;

		// 지면에 착지 시 하향 중력 속도 초기화
		m_GravityVelocity.y = max(m_GravityVelocity.y, 0.f);

		// 지면 판정 갱신
		m_IsGround = true;
	}

	// 최종 위치 적용
	Transform()->SetRelativePos(vPos);
}

void PlayerCharacter::BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::BoxCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
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

	if (InCollider->GetColliderType() == EColliderType::RayCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		m_CollObject = OtherCollider->GetOwner();
	}

	if (InCollider->GetColliderType() == EColliderType::BoxCollider &&
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
}

void PlayerCharacter::Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::BoxCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* Collider = static_cast<FBoxCollider*>(InCollider);
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		// 트리거용 충돌체면 해당 코드 사용 x
		if (OtherCollider->IsTrigger())
		{
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

	if (InCollider->GetColliderType() == EColliderType::BoxCollider &&
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

void PlayerCharacter::EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::RayCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		if (m_CollObject == OtherCollider->GetOwner())
		{
			m_CollObject = nullptr;
		}
	}
}

// void PlayerCharacter::BeginOverlap(FBoxCollider* PCollider, CGameObject* POtherObject, FLandScape* POtherCollider)
// {
// 	Vec3 pPos = Transform()->GetRelativePos();
//
// 	//// 충돌 전 위치로 약간 되돌림
// 	//Vec3 prevPos = pPos - m_Velocity * DT;
//
// 	// 지형의 노말 벡터를 얻음
// 	Vec3 LandNormal = POtherCollider->GetWorldPosLandNormal(pPos);
//
// 	Transform()->SetRelativePos(pPos);
//
// 	// 노말이 유효하면 사용
// 	if (LandNormal.Length() > 0.001f)
// 	{
// 		// 충돌 노말 추가
// 		m_vecCollisionNormal.push_back(LandNormal);
//
// 		//// 파고듬 방지
// 		//float dotProduct = m_Velocity.Dot(LandNormal);
// 		//if (dotProduct < 0.f)
// 		//{
// 		//	// 더 위치 옮겨봄
// 		//	Vec3 adjustedPos = pPos + LandNormal * 0.05f;
// 		//	Transform()->SetRelativePos(adjustedPos);
// 		//}
// 	}
// }
//
// void PlayerCharacter::Overlap(FBoxCollider* PCollider, CGameObject* POtherObject, FLandScape* POtherCollider)
// {
// 	Vec3 pPos = Transform()->GetRelativePos();
//
// 	//// 충돌 전 위치로 약간 되돌림
// 	//Vec3 prevPos = pPos - m_Velocity * DT;
//
// 	// 지형의 노말 벡터를 얻음
// 	Vec3 LandNormal = POtherCollider->GetWorldPosLandNormal(pPos);
//
// 	// 노말이 유효하면 사용
// 	if (LandNormal.Length() > 0.001f)
// 	{
// 		// 충돌 노말 추가
// 		m_vecCollisionNormal.push_back(LandNormal);
//
// 		//// 지속적으로 파고듬 방지
// 		//float dotProduct = m_Velocity.Dot(LandNormal);
// 		//if (dotProduct < 0.f)
// 		//{
// 		//	// 혹시 모르니 더 옳김
// 		//	Vec3 adjustedPos = pPos + LandNormal * 0.05f;
// 		//	Transform()->SetRelativePos(adjustedPos);
// 		//}
// 	}
// }
//
// void PlayerCharacter::EndOverlap(FBoxCollider* PCollider, CGameObject* POtherObject, FLandScape* POtherCollider)
// {
// }
