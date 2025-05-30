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
	: CScript(_Type)
	, m_MainCamera(nullptr)
	, m_CamScript(nullptr)
	, m_EquippedOwner(nullptr)
	, m_CurKey()
	, m_CurKeyState()
	, m_WeaponType()
	, m_vDesPos(Vec3(0.f, 0.f, 0.f))
	, m_bEnemy(false)
	, m_bIsEquipped(false)
	, m_bTransition(false)

{

}

WeaponController::~WeaponController()
{
}

void WeaponController::Begin()
{
	m_MainCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
	m_CamScript = static_cast<CameraController*>(GetScriptWithType(m_MainCamera, SCRIPT_TYPE::CAMERASCRIPT));
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

void WeaponController::AdjustFPSPos()
{
	// 현재 FPS 모드라면 위치를 조정한다.
	if (!m_CamScript->GetFlag(TPS))
	{
		Vec3 vCamPos = m_MainCamera->Transform()->GetRelativePos();
		Vec3 vCamRot = m_MainCamera->Transform()->GetRelativeRotation();

		Vec3 vFoward = m_MainCamera->Transform()->GetWorldDir(DIR_TYPE::FRONT);
		Vec3 vRight = m_MainCamera->Transform()->GetWorldDir(DIR_TYPE::RIGHT);		

		// 카메라의 위아래 회전값에 따라 앞뒤로 위치를 조정해준다
		Vec3 pitchMove = vFoward * -vCamRot.x;

	
		// 무기의 회전값을 보정해준다.
		Vec3 vRot = Vec3(0.f, 0.f, 0.f);
		vRot.y = vCamRot.y - 90.f;
		vRot.z = -vCamRot.x;

		Vec3 vOffset = Vec3(0.f, 0.f, 0.f);
		Vec3 vPos = Vec3(0.f, 0.f, 0.f);

		// 정조준일 때의 위치
		if (m_CamScript->GetFlag(ADS))
		{
			vOffset = vFoward * 80.f;
			vPos = vCamPos + vOffset + pitchMove;
			vPos.y -= 200.f;

			static float leanAngle = 0.f;
			// 정조준 도준 기울이기 입력이 들어오면 무기의 회전값 변경
			if (m_CurKey == KEY::Q && m_CurKeyState == KEY_STATE::PRESSED)
			{
				leanAngle = FloatLerp(leanAngle, 10.f, 10.f);
			}
			else if (m_CurKey == KEY::E && m_CurKeyState == KEY_STATE::PRESSED)
			{
				leanAngle = FloatLerp(leanAngle, -10.f, 10.f);
			}
			else
			{
				leanAngle = FloatLerp(leanAngle, 0.f, 10.f);
			}

			vRot.x += leanAngle;
		}
		// 아닐 때의 위치
		else
		{	
			// 무기를 카메라 기준 오른쪽 앞에 위치할수 있도록 해준다.
			vOffset = vRight * 220.f + vFoward * 125.f;
			vPos = vCamPos + vOffset + pitchMove;
			vPos.y -= 200.f;
		}

		if (m_CurKey == KEY::RBTN && m_CurKeyState == KEY_STATE::TAP)
		{
			m_vDesPos = vPos;
			m_bTransition = true;
			ClearKey();
		}

		if (m_bTransition)
		{
			TransitionPos(m_vDesPos);
		}
		else
		{
			Transform()->SetRelativePos(vPos);
			Transform()->SetRelativeRotation(vRot);
		}
	}
}

void WeaponController::TransitionPos(Vec3 _DesPos)
{
	Vec3 vPos = Transform()->GetRelativePos();

	float fTimes = 0.95f;
	float fChangeSpeed = 20.f;

	vPos = Vec3Lerp(vPos, _DesPos, fChangeSpeed);
	Transform()->SetRelativePos(vPos);

	// 특정 구간에 도달하면 변환을 종료시킨다.
	if (fabs(vPos.x - _DesPos.x) < 10.0f && fabs(vPos.x - _DesPos.x) < 10.0f)
	{
		Transform()->SetRelativePos(vPos);
		m_bTransition = false;
	}
}


void WeaponController::SaveComponent(FILE* _File)
{
	fwrite(&m_WeaponType, sizeof(int), 1, _File);
}

void WeaponController::LoadComponent(FILE* _File)
{
	fread(&m_WeaponType, sizeof(int), 1, _File);
}


