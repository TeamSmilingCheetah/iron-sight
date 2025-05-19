#include "pch.h"

#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"

#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Projectile/Public/MissileProjectile.h"



WeaponController::WeaponController(SCRIPT_TYPE _Type)
	: CScript(static_cast<UINT>(_Type))
	, m_MainCamera(nullptr)
	, m_EquippedOwner(nullptr)
	, m_bEnemy(false)
{

}

WeaponController::~WeaponController()
{
}

void WeaponController::Begin()
{
	m_MainCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
}

Vec3 WeaponController::GetFireDir()
{
	// 적일경우 발사 방향 결정
	if (m_bEnemy)
	{
		// 적의 전방 방향 가져오기
		Vec3 vEnemyDir = m_EquippedOwner->Transform()->GetWorldDir(DIR_TYPE::FRONT);

		vEnemyDir.Normalize();

		return vEnemyDir;
	}
	// 플레이어일 경우 발사 방향 결정
	else
	{
		// 카메라의 방향 정보
		Vec3 vCameraPos = m_MainCamera->Transform()->GetRelativePos();
		Vec3 vCameraRot = m_MainCamera->Transform()->GetRelativeRotation();
		float radX = XMConvertToRadians(vCameraRot.x);
		float radY = XMConvertToRadians(vCameraRot.y);
		float radZ = XMConvertToRadians(vCameraRot.z);
		Vec3 vCameraDir = Vec3(0.f, 0.f, 1.f);
		Matrix matCameraRot = XMMatrixRotationRollPitchYaw(radX, radY, radZ);
		vCameraDir = XMVector3TransformNormal(vCameraDir, matCameraRot);
		vCameraDir.Normalize();

		// Player의 위치와 방향 정보
		Vec3 vPlayerPos = m_EquippedOwner->Transform()->GetRelativePos();
		Vec3 vPlayerRot = m_EquippedOwner->Transform()->GetRelativeRotation();
		float playerRadX = XMConvertToRadians(vPlayerRot.x);
		float playerRadY = XMConvertToRadians(vPlayerRot.y);
		float playerRadZ = XMConvertToRadians(vPlayerRot.z);
		Vec3 vPlayerDir = Vec3(0.f, 0.f, 1.f);
		Matrix matPlayerRot = XMMatrixRotationRollPitchYaw(playerRadX, playerRadY, playerRadZ);
		vPlayerDir = XMVector3TransformNormal(vPlayerDir, matPlayerRot);
		vPlayerDir.Normalize();

		// 카메라 정중앙이 바라보는곳에서 임의의 타켓 포인트를 구한다.

		Vec3 vTargetPoint = vCameraPos + vCameraDir * 100000.f;

		// 해당 타켓 포인트를 바라보는 방향을 구한다.
		Vec3 vFinalDir = vTargetPoint - vPlayerPos;
		vFinalDir.Normalize();

		return vFinalDir;
	}
}

void WeaponController::ClearKey()
{
	m_CurKey = KEY::END;
	m_CurKeyState = KEY_STATE::NONE;
}


void WeaponController::SaveComponent(FILE* _File)
{
	fwrite(&m_WeaponType, sizeof(int), 1, _File);
}

void WeaponController::LoadComponent(FILE* _File)
{
	fread(&m_WeaponType, sizeof(int), 1, _File);
}


