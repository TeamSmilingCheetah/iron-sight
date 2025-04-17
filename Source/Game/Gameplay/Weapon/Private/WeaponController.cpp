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
	, m_EquippedOwner(nullptr)
{

}

WeaponController::~WeaponController()
{
}

Vec3 WeaponController::GetFireDir()
{

	CGameObject* pCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");

	// 카메라의 방향 정보
	Vec3 vCameraPos = pCamera->Transform()->GetRelativePos();
	Vec3 vCameraRot = pCamera->Transform()->GetRelativeRotation();
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


	// 총알의 시작 위치를 보정해준다
	Vec3 vSpawnPos = vPlayerPos;
	//vSpawnPos.x += 350.f * vPlayerDir.x;
	//vSpawnPos.z -= 950.f;
	vSpawnPos.y += 800.f;

	// 카메라 정중앙이 바라보는곳에서 임의의 타켓 포인트를 구한다.
	Vec3 vTargetPoint = vCameraPos + vCameraDir * 100000.f;

	// 해당 타켓 포인트를 바라보는 방향을 구한다.
	Vec3 vFinalDir = vTargetPoint - vPlayerPos;
	vFinalDir.Normalize();

	return vFinalDir;
}

void WeaponController::ClearKey()
{
	m_CurKey = KEY::END;
	m_CurKeyState = KEY_STATE::NONE;
}

void WeaponController::SaveComponent(FILE* _File)
{
}

void WeaponController::LoadComponent(FILE* _File)
{
}


