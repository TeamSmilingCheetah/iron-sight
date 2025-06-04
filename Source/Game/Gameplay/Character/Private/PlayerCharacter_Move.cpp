#include "pch.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Rendering/Device/CDevice.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"

#include "Engine/Runtime/Public/Actor/CLayer.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"

#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"

void PlayerCharacter::PlayerMove()
{
	// 힘을 0 으로 초기화
	m_Force = Vec3(0.f, 0.f, 0.f);

	Vec3 vPos = Transform()->GetRelativePos();

	// 이동할 방향 연산
	MoveCalcul();

	// 중력 연산
	gravityCalcul();

	// 충돌 연산
	ColliderCalcul();

	vPos += m_Velocity;
	Transform()->SetRelativePos(vPos);

	// 충돌벡터 초기화
	m_vecCollisionNormal.clear();
}

void PlayerCharacter::MoveCalcul()
{
	// 방향키입력 확인
	Vec3 vRot = Transform()->GetRelativeRotation();
	//float radian = vRot.y * XM_PI / 180.f;
	
	//Vec3 vFowardDir = { -sinf(radian), 0.f, -cosf(radian) };
	Vec3 vFowardDir = Transform()->GetWorldDir(DIR_TYPE::FRONT);
	//Vec3 vRightDir = { -cosf(radian), 0.f, sinf(radian) };
	Vec3 vRightDir = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vInputDir = { 0.f,0.f,0.f };

	// 이전 틱의 이동방향저장
	Vec3 vPrevVelocityDir = m_Velocity;
	if (vPrevVelocityDir.Length() > 0.001f) {
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
		!m_bLean)
	{
		ForceScar = 60.f;
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
	else if(m_IsGround)
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

void PlayerCharacter::gravityCalcul()
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
		m_GravityVelocity += (gravityDir * m_GravityAccel * m_Mass) * DT;

		// 최대 중력속도 제한
		if (m_GravityVelocity.y < -m_GravityMaxSpeed)
			m_GravityVelocity.y = -m_GravityMaxSpeed;
	}
	else
	{
		// 땅위이며 아래로 내려가는것이 아니라면 속도 0으로 설정
		if (m_GravityVelocity.y < 0.f)
		{
			m_GravityVelocity = Vec3(0.f, 0.f, 0.f);
		}
	}

	// 점프 기능
	if (m_IsGround && KEY_TAP(KEY::X))
	{
		m_GravityVelocity += Vec3(0.f,1.f,0.f) * m_JumpPower;
	}

	m_Velocity += m_GravityVelocity;
}

void PlayerCharacter::ColliderCalcul()
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




void PlayerCharacter::BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	// 트리거용 충돌체면 해당 코드 사용 x
	if (_OtherCollider->IsTrigger())
	{
		return;

	}
	else
	{
		Vec3 pPos = Transform()->GetRelativePos();

		Vec3 hitNormal = _Collider->GetHitNormal();
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
			Vec3 otherPos = _OtherObject->Transform()->GetRelativePos();
			Vec3 normal = myPos - otherPos;
			normal.Normalize();

			// 충돌 벡터 추가
			m_vecCollisionNormal.push_back(normal);
		}
	}
}

void PlayerCharacter::Overlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	// 트리거용 충돌체면 해당 코드 사용 x
	if (_OtherCollider->IsTrigger())
	{
		
	}
	else
	{
		Vec3 hitNormal = _Collider->GetHitNormal();
		// 노말이 유효하면 사용
		if (hitNormal.Length() > 0.001f)
		{
			// 충돌 노말 추가
			m_vecCollisionNormal.push_back(hitNormal);
		}
		else
		{
			Vec3 myPos = Transform()->GetRelativePos();
			Vec3 otherPos = _OtherObject->Transform()->GetRelativePos();
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
				Vec3 otherPos = _OtherObject->Transform()->GetRelativePos();
				Vec3 normal = myPos - otherPos;
				normal.Normalize();

				// 충돌 벡터 추가
				m_vecCollisionNormal.push_back(normal);
			}
		}
	}
}

void PlayerCharacter::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
}


void PlayerCharacter::BeginOverlap(CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider)
{
	m_CollObject = _OtherObject;
}

void PlayerCharacter::Overlap(CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider)
{
	if (_3DCollider->IsTrigger())
	{
		
	}
}

void PlayerCharacter::EndOverlap(CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider)
{
	if (m_CollObject == _OtherObject)
		m_CollObject = nullptr;
}



void PlayerCharacter::BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider)
{

	Vec3 pPos = Transform()->GetRelativePos();

	//// 충돌 전 위치로 약간 되돌림
	//Vec3 prevPos = pPos - m_Velocity * DT;

	// 지형의 노말 벡터를 얻음
	Vec3 LandNormal = _OtherCollider->GetWorldPosLandNormal(pPos);

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

void PlayerCharacter::Overlap(CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider)
{
	Vec3 pPos = Transform()->GetRelativePos();

	//// 충돌 전 위치로 약간 되돌림
	//Vec3 prevPos = pPos - m_Velocity * DT;

	// 지형의 노말 벡터를 얻음
	Vec3 LandNormal = _OtherCollider->GetWorldPosLandNormal(pPos);

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

void PlayerCharacter::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider)
{
}
