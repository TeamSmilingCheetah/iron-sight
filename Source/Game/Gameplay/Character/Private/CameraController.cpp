#include "pch.h"

#include "Game/Gameplay/Character/Public/CameraController.h"

#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

CameraController::CameraController()
	: CScript(static_cast<UINT>(SCRIPT_TYPE::CAMERASCRIPT))
	, m_CameraSpeed(100.f)
	, m_bSearchRecover(false)
	, m_bRight(true)
	, m_bZoomRecover(false)
	, m_bZoom(false)
	, m_bChangeFocus(false)
{
}

CameraController::~CameraController()
{
}

void CameraController::Tick()
{
	if (ORTHOGRAPHIC == Camera()->GetProjType())
		CameraOrthgraphicMove();
	else
		CameraPerspectiveMove();
}

void CameraController::CameraOrthgraphicMove()
{
	Vec3 vWorldPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vWorldPos.y += DT * m_CameraSpeed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vWorldPos.y -= DT * m_CameraSpeed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vWorldPos.x -= DT * m_CameraSpeed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vWorldPos.x += DT * m_CameraSpeed;
	}

	Transform()->SetRelativePos(vWorldPos);


	float Scale = Camera()->GetScale();

	if (KEY_PRESSED(KEY::NUMPAD_0))
	{
		Scale += DT;
	}

	if (KEY_PRESSED(KEY::NUMPAD_1))
	{
		Scale -= DT;
		if (Scale < 0.1f)
			Scale = 0.1f;
	}

	Camera()->SetScale(Scale);
}

void CameraController::CameraPerspectiveMove()
{
	// Player를 찾아 Player에 카메라를 부착시킨다.
	CGameObject* pPlayer = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");

	Vec3 vCameraPos = Transform()->GetRelativePos();
	Vec3 vCameraRot = Transform()->GetRelativeRotation();
	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vPlayerRot = pPlayer->Transform()->GetRelativeRotation();


	float RadianPitch = vCameraRot.x * XM_PI / 180.f;
	float DistanceRatio = 1000.f;
	float HeightRatio = 2000.f;

	static float lateralOffset = 300.f;
	static float adjustDistance = DistanceRatio * cosf(RadianPitch);
	static float adjustHeight = HeightRatio + DistanceRatio * sinf(RadianPitch);


	// TPS
	// 
	// Player가 앞을 보고 있으면 Dir는 - , 아니면 +
	float radian_PlayerRotY = vPlayerRot.y * XM_PI / 180.f;

	// Player의 후방 벡터
	Vec3 vBackDir = { sinf(radian_PlayerRotY), 0.f, cosf(radian_PlayerRotY) };

	// Player의 오른쪽 벡터
	Vec3 vRightDir = { -cosf(radian_PlayerRotY), 0.f, sinf(radian_PlayerRotY) };


	// 우클릭을 누르고 있는 동안 줌 한다.
	static float OriginDistance = 0.f;
	static float OriginHeight = 0.f;
	static float ObjectiveDistance = 0.f;
	static float ObjectiveHeight = 0.f;

	static float ObjectiveLateralOff = 0.f;

	//
	// 줌
	// 
	// 줌 회복 상태가 아니라면 줌을 활성화 한다.
	if (!m_bZoomRecover)
	{
		// 줌 활성화
		if (KEY_TAP(KEY::RBTN))
		{
			ObjectiveDistance = adjustDistance * 0.5f;
			ObjectiveHeight = adjustHeight * 0.9f;
			OriginDistance = adjustDistance;
			OriginHeight = adjustHeight;
			lateralOffset = 500.f;
			m_bZoom = true;
		}

		// 카메라위 위치를 조정하여 줌해준다.
		if (KEY_PRESSED(KEY::RBTN))
		{
			if (ObjectiveDistance < adjustDistance)
			{
				adjustDistance = adjustDistance + (ObjectiveDistance - adjustDistance) * 50.f * DT;
			}
			if (ObjectiveHeight < adjustHeight)
			{
				adjustHeight = adjustHeight + (ObjectiveHeight - adjustHeight) * 50.f * DT;
			}

			
			// 기울이기에 따라 줌 위치를 바꿔준다.
			if (KEY_TAP(KEY::Q))
			{
				m_bRight = false;
				m_bChangeFocus = true;
				ObjectiveLateralOff = -500.f;
			}
			if (KEY_TAP(KEY::E))
			{
				m_bRight = true;
				m_bChangeFocus = true;
				ObjectiveLateralOff = 500.f;
			}
		}

		// 줌 회복을 활성화 해준다.
		if (KEY_RELEASED(KEY::RBTN))
		{
			m_bZoomRecover = true;
			m_bZoom = false;
		}
	}
	// 줌 회복 
	else
	{
		// 기존 카메라 위치로 회복시켜준다.
		if (adjustDistance < OriginDistance)
		{
			adjustDistance = adjustDistance + (OriginDistance - adjustDistance) * 50.f * DT;
		}
		if (adjustHeight < OriginHeight)
		{
			adjustHeight = adjustHeight + (OriginHeight - adjustHeight) * 50.f * DT;
		}
		if (abs(adjustHeight - OriginHeight) < 5.f && abs(adjustDistance - OriginDistance) < 5.f)
		{
			m_bZoomRecover = false;
		}
	}


	// 좌우 포커스 변경
	if (m_bChangeFocus)
	{
		lateralOffset = lateralOffset + (ObjectiveLateralOff - lateralOffset) * 50.f * DT;
		if (abs(lateralOffset - ObjectiveLateralOff) < 5.f)
		{
			m_bChangeFocus = false;
		}
	}

	// Player 위치에 보정값을 더해 위치를 고정시킨다.
	vCameraPos.x = vPlayerPos.x + adjustDistance * vBackDir.x;
	vCameraPos.y = vPlayerPos.y + adjustHeight;
	vCameraPos.z = vPlayerPos.z + adjustDistance * vBackDir.z;

	// Player가 Camera상에서 약간 왼쪽에 위치하게끔 보정한다.
	vCameraPos.x += lateralOffset * vRightDir.x;
	vCameraPos.z += lateralOffset * vRightDir.z;

	//
	// Search
	//
	static float OriginRotY = 0.f;
	static float OriginRotX = 0.f;
	if (KEY_TAP(KEY::Z))
	{
		// 현재의 RotY을 기억해놓는다.
		OriginRotY = vCameraRot.y;
		OriginRotX = vCameraRot.x;
	}
	if (KEY_RELEASED(KEY::Z))
	{
		m_bSearchRecover = true;
	}

	if (KEY_PRESSED(KEY::Z))
	{
		int a = 0;
	}
	else
	{
		// 회복중이 아닌 일반시
		if (!m_bSearchRecover)
		{
			vCameraRot.y = vPlayerRot.y + 180.f;
   			Transform()->SetRelativeRotation(vCameraRot);
		}
		// 회복중일때 저장해놓은 위치로 카메라를 원위치 시킨다.
		else
		{
			float RecoverSpeed = 10.f;

			vCameraRot.y = vCameraRot.y + (OriginRotY - vCameraRot.y) * RecoverSpeed * DT;
			vCameraRot.x = vCameraRot.x + (OriginRotX - vCameraRot.x) * RecoverSpeed * DT;
			Transform()->SetRelativeRotation(vCameraRot);

			// 특정 구간에 도달하면 회복을 종료시킨다.
			if(fabs(vCameraRot.y - OriginRotY) < 1.f && fabs(vCameraRot.x - OriginRotX) < 1.f)
			{
				m_bSearchRecover = false;
			}			
		}
	}

	Transform()->SetRelativePos(vCameraPos);
	


	// FPS
	//if (KEY_PRESSED(KEY::LSHIFT))
	//	Speed *= 5.f;

	//Vec3 vFront = Transform()->GetLocalDir(DIR_TYPE::FRONT);
	//Vec3 vRight = Transform()->GetLocalDir(DIR_TYPE::RIGHT);

	//Vec3 vPos = Transform()->GetRelativePos();

	//if (KEY_PRESSED(KEY::W))
	//	vPos += vFront * DT * Speed;
	//if (KEY_PRESSED(KEY::S))
	//	vPos -= vFront * DT * Speed;
	//if (KEY_PRESSED(KEY::A))
	//	vPos -= vRight * DT * Speed;
	//if (KEY_PRESSED(KEY::D))
	//	vPos += vRight * DT * Speed;

	//Transform()->SetRelativePos(vPos);

	//// 마우스 방향에 따른 오브젝트 회전
	//if (KEY_PRESSED(KEY::RBTN))
	//{
	//	Vec3 vRot = Transform()->GetRelativeRotation();

	//	Vec2 vDir = CKeyMgr::GetInst()->GetMouseDir();
	//	vRot.y += vDir.x * DT * 15.f;
	//	vRot.x += vDir.y * DT * 10.f;

	//	Transform()->SetRelativeRotation(vRot);
	//}
}

void CameraController::SaveComponent(FILE* _File)
{
	fwrite(&m_CameraSpeed, sizeof(float), 1, _File);
}

void CameraController::LoadComponent(FILE* _File)
{
	fread(&m_CameraSpeed, sizeof(float), 1, _File);
}
