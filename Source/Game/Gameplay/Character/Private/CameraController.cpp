#include "pch.h"

#include "Game/Gameplay/Character/Public/CameraController.h"

#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/GamePlay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"


CameraController::CameraController()
	: CScript(SCRIPT_TYPE::CAMERASCRIPT)
	, m_Player(nullptr)
	, m_PlayerScript(nullptr)
	, m_InventoryScript(nullptr)
	, m_CameraSpeed(100.f)
	, m_CameraPos(Vec3(0.f,0.f,0.f))
	, m_CameraRot(Vec3(0.f,0.f,0.f))
	, m_PlayerPos(Vec3(0.f,0.f,0.f))
	, m_PlayerRot(Vec3(0.f,0.f,0.f))
	, m_CameraFlag(0)
	, m_AccTime(0.f)
	, m_RecoilTime(0.f)
	, m_RecoilAmount_vertical(0.f)
	, m_RecoilAmount_horizontal(0.f)
	, m_LateralOffset(300.f)
	, m_ObjectiveLateralOff(0.f)
	, m_AdjustNormalDistance(0.f)
	, m_AdjustNormalHeight(0.f)
	, m_AdjustFinalDistance(0.f)
	, m_AdjustFinalHeight(0.f)
	, m_ObjectiveShoulderDistance(0.f)
	, m_ObjectiveShoulderHeight(0.f)
{
}

CameraController::~CameraController()
{
}

void CameraController::Begin()
{
	// 기본 카메라 모드 설정
	m_CameraFlag |= TPS;

	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));
	m_InventoryScript = static_cast<InventoryController*>(GetScriptWithType(m_Player, SCRIPT_TYPE::INVENTORYSCRIPT));
}

void CameraController::Tick()
{
	if (ORTHOGRAPHIC == Camera()->GetProjType())
		CameraOrthgraphicMove();
	else
		CameraPerspectiveMove();


	// 소리테스트 위치 업데이트
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vforward = Transform()->GetWorldDir(DIR_TYPE::FRONT);
	Vec3 vUp = Transform()->GetWorldDir(DIR_TYPE::UP);
	CSoundMgr::GetInst()->UpdateListener(vPos, vforward, vUp);
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
	int iWeaponIdx = m_InventoryScript->GetCurSlotIdx();

	m_CameraPos = Transform()->GetRelativePos();
	m_CameraRot = Transform()->GetRelativeRotation();
	m_PlayerPos = m_Player->Transform()->GetRelativePos();
	m_PlayerRot = m_Player->Transform()->GetRelativeRotation();

	// 카메라 전환
	if (KEY_TAP(KEY::V))
	{
		if (!(m_CameraFlag & SHOULDER) && !(m_CameraFlag & ADS))
		{
			ChangePS(!(m_CameraFlag & TPS));
		}
	}

	// TPS
 	if (m_CameraFlag & TPS)
	{
		// TPS의 카메라 기본위치들 변수 업데이트
		UpdateTPSCameraAdjustments();

		//
		// 견착과 줌
		//

		// 줌 회복 상태가 아니거나 둘러보기 상태가 아니라면 줌을 활성화 한다.
		if (!(m_CameraFlag & SHOULDER_RECOVER)&& !(m_CameraFlag & SEARCH) && iWeaponIdx <= SECONDARY_FIRST)
		{
			HandleRightClickInput();

			// 견착 상태
			if (m_CameraFlag & SHOULDER)
			{
				UpdateShoulderMode();
			}

			// 줌 상태
			if (m_CameraFlag & ADS)
			{
				// TPS 비활성화, FPS 활성화
				m_CameraFlag |= WAS_TPS;
				ChangePS(false);
			}
		}

		// 줌 회복
		else
		{
			UpdateShoulderRecover();
		}

		if (!(m_CameraFlag & SHOULDER))
		{
			UpdateTPSLean();
		}			

		// Player 위치에 보정값을 더해 위치를 고정시킨다.
		m_CameraPos.x = m_PlayerPos.x + m_AdjustFinalDistance * m_Player->Transform()->GetWorldDir(DIR_TYPE::FRONT).x;
		m_CameraPos.y = m_PlayerPos.y + m_AdjustFinalHeight;
		m_CameraPos.z = m_PlayerPos.z + m_AdjustFinalDistance * m_Player->Transform()->GetWorldDir(DIR_TYPE::FRONT).z;


		//  Player가 Camera상에서 약간 왼쪽에 위치하게끔 보정한다.
		m_CameraPos.x += m_LateralOffset * -m_Player->Transform()->GetWorldDir(DIR_TYPE::RIGHT).x;
		m_CameraPos.z += m_LateralOffset * -m_Player->Transform()->GetWorldDir(DIR_TYPE::RIGHT).z;
			
		// Search
		UpdateSearchMode();
	}


	// FPS
	else
	{
		// 견착모드였다면 해제한다.
		m_CameraFlag &= ~SHOULDER;

		// 카메라의 위치를 Player의 위치로 바꿔준다.
		m_CameraPos.x = m_PlayerPos.x;
		m_CameraPos.y = m_PlayerPos.y + 1800.f;
		m_CameraPos.z = m_PlayerPos.z;

		m_CameraRot.y = m_PlayerRot.y + 180.f;

		UpdateFPSLean();

		// 줌 활성화
		if (iWeaponIdx <= SECONDARY_FIRST)
		{
			if (KEY_TAP(KEY::RBTN))
			{
				// TPS에서 줌으로 넘어온 경우 줌을 풀때 TPS로 넘어간다.
				if ((m_CameraFlag & WAS_TPS) && (m_CameraFlag & ADS))
				{
					ChangePS(true);
				}
				m_CameraFlag |= CHANGE_FOV;
				m_CameraFlag ^= ADS;
			}
		}

	}

	if (m_CameraFlag & CHANGE_FOV)
	{
		ApplyZoom(m_CameraFlag & ADS);
	}


	//
	// 반동
	//
	// 플레이어가 총기를 발사하는 중이라면s
	if (m_PlayerScript->IsShot())
	{
		m_RecoilTime += DT;

		ApplyRecoil();
	}



	Transform()->SetRelativeRotation(m_CameraRot);
	Transform()->SetRelativePos(m_CameraPos);
}

void CameraController::ApplyZoom(bool _IsADS)
{
	if (_IsADS)
	{
		float fCurFOV = GetOwner()->Camera()->GetFOV();
		// 플레이어의 배율에 따라 카메라의 확대 정도를 조절한다.
		//m_Player->

		float fTimes = 0.f;
		//if()
		fTimes = 0.95f;
		float fDestFOV = (XM_PI / 2.f) * fTimes;

		float fChangeSpeed = 30.f;

		fCurFOV = fCurFOV + (fDestFOV - fCurFOV) * fChangeSpeed * DT;
		GetOwner()->Camera()->SetFOV(fCurFOV);
		// 특정 구간에 도달하면 변환을 종료시킨다.
		if (fabs(fCurFOV - fDestFOV) < 0.01f && fabs(fCurFOV - fDestFOV) < 0.01f)
		{
			GetOwner()->Camera()->SetFOV(fDestFOV);
			m_CameraFlag &= ~CHANGE_FOV;
		}
	}
	else
	{
		float fCurFOV = GetOwner()->Camera()->GetFOV();
		float fDestFOV = XM_PI / 2.f;

		float fChangeSpeed = 30.f;

		fCurFOV = fCurFOV + (fDestFOV - fCurFOV) * fChangeSpeed * DT;
		GetOwner()->Camera()->SetFOV(fCurFOV);

		// 특정 구간에 도달하면 변환을 종료시킨다.
		if (fabs(fCurFOV - fDestFOV) < 0.01f && fabs(fCurFOV - fDestFOV) < 0.01f)
		{
			GetOwner()->Camera()->SetFOV(fDestFOV);
			m_CameraFlag &= ~CHANGE_FOV;
		}
	}
}

void CameraController::ApplyRecoil()
{
	if (0.05f < m_RecoilTime)
	{
		m_RecoilTime = 0.0f;  // 타이머 초기화

		// 반동 적용
		float recoilPower_vertical = 1.2f;
		float recoilPower_horizontal = 0.5f;
		float maxRecoli_vertical = 1.5f;
		float maxRecoli_horizontal = 1.f;
		float maxTotalYaw = 10.f;

		// 현재 Player가 들고 있는 무기의 반동 변수 값을 가져온다.
		GunController* pGunScript = static_cast<GunController*>(m_InventoryScript->GetCurWeaponController());
		recoilPower_horizontal = pGunScript->GetHorizontalPower();
		recoilPower_vertical = pGunScript->GetVerticalPower();


		// 랜덤한 수직 반동값을 만들어준다
		float randomRecoil_vertical = (rand() % 100 / 100.f) * maxRecoli_vertical * recoilPower_vertical;
		m_RecoilAmount_vertical = m_RecoilAmount_vertical * (1.f - 0.1f) + randomRecoil_vertical * 0.1f;


		// 랜덤한 수평 반동값을 반들어준다
		float randomRecoil_horizontal = ((rand() % 200) - 100.f) / 100.f * maxRecoli_horizontal * recoilPower_horizontal;

		// 반동값 보정 (한쪽으로 치우치지 않게)
		if (abs(m_RecoilAmount_horizontal + randomRecoil_horizontal) > maxTotalYaw)
		{
			randomRecoil_horizontal = -randomRecoil_horizontal * 0.5f;
		}
		m_RecoilAmount_horizontal = m_RecoilAmount_horizontal * (1.f - 0.7f) + randomRecoil_horizontal * 0.7f;


		m_CameraRot.x -= m_RecoilAmount_vertical;
		m_PlayerRot.y -= m_RecoilAmount_horizontal;
		if (m_CameraRot.x < -90.f)
		{
			m_CameraRot.x = -90.f;
		}

		Transform()->SetRelativeRotation(m_CameraRot);
		m_Player->Transform()->SetRelativeRotation(m_PlayerRot);
	}
}

void CameraController::UpdateTPSCameraAdjustments()
{
	float RadianPitch = m_CameraRot.x * XM_PI / 180.f;
	float DistanceRatio = 1000.f;
	float HeightRatio = 2000.f;

	// 평상시에 카메라 있을 위치를 계산해 놓는다.
	m_AdjustNormalDistance = DistanceRatio * cosf(RadianPitch);
	m_AdjustNormalHeight = HeightRatio + DistanceRatio * sinf(RadianPitch);


	// 평상시 상태에서 위치 값
	if (!(m_CameraFlag & SHOULDER))
	{
		m_AdjustFinalDistance = m_AdjustNormalDistance;
		m_AdjustFinalHeight = m_AdjustNormalHeight;
	}
	// 견착상태에서 카메라가 향할 위치를 계산해놓는다.
	else
	{
		m_ObjectiveShoulderDistance = m_AdjustNormalDistance * 0.5f;
		m_ObjectiveShoulderHeight = m_AdjustNormalHeight * 0.9f;

		m_AdjustFinalDistance = FloatLerp(m_AdjustFinalDistance, m_ObjectiveShoulderDistance, 50.f);
		m_AdjustFinalHeight = FloatLerp(m_AdjustFinalHeight, m_ObjectiveShoulderHeight, 50.f);
	}

}

void CameraController::HandleRightClickInput()
{
	// 인벤토리가 열려있을 땐 입력 방지
	if (!m_PlayerScript->IsInventoryOpened())
	{
		if (KEY_TAP(KEY::RBTN))
		{
			m_CameraFlag |= CLICKED_FIRST;
		}
		if (KEY_PRESSED(KEY::RBTN))
		{
			m_AccTime += DT;

			// 특정 시간 이상 누른상태라면 견착모드로 진입
			if (0.1f < m_AccTime)
			{
				m_CameraFlag |= SHOULDER;
			}
		}
		if (KEY_RELEASED(KEY::RBTN))
		{
			m_AccTime = 0.f;
			// 특정 시간 이하, 즉 우클릭을 클릭만 했을때는 정조준 진입
			if (!(m_CameraFlag & SHOULDER) && !(m_CameraFlag & WAS_TPS))
			{
				m_CameraFlag |= CHANGE_FOV;
				m_CameraFlag |= ADS;
			}
			else if ((m_CameraFlag & WAS_TPS))
			{
				m_CameraFlag &= ~WAS_TPS;
			}
		}
	}
}

void CameraController::UpdateShoulderMode()
{
	if ((m_CameraFlag & CLICKED_FIRST))
	{
		m_LateralOffset = 500.f;
		m_CameraFlag &= ~CLICKED_FIRST;
	}


	// 카메라위 위치를 조정하여 줌해준다.
	if (KEY_PRESSED(KEY::RBTN))
	{
		// 기울이기에 따라 줌 위치를 바꿔준다.
		if (KEY_TAP(KEY::Q))
		{
			m_CameraFlag &= ~RIGHT;
			m_CameraFlag |= CHANGE_FOCUS;
			m_ObjectiveLateralOff = -500.f;
		}
		if (KEY_TAP(KEY::E))
		{
			m_CameraFlag |= RIGHT;
			m_CameraFlag |= CHANGE_FOCUS;
			m_ObjectiveLateralOff = 500.f;
		}
	}

	// 줌 회복을 활성화 해준다.
	if (KEY_RELEASED(KEY::RBTN))
	{
		m_CameraFlag |= SHOULDER_RECOVER;
		m_CameraFlag &= ~SHOULDER;
		m_LateralOffset = 300.f;
	}

	// 좌우 포커스 변경
	if (m_CameraFlag & CHANGE_FOCUS)
	{
		m_LateralOffset = FloatLerp(m_LateralOffset, m_ObjectiveLateralOff, 50.f);
		if (abs(m_LateralOffset - m_ObjectiveLateralOff) < 5.f)
		{
			m_CameraFlag &= ~CHANGE_FOCUS;
		}
	}
}

void CameraController::UpdateSearchMode()
{
	static float OriginRotY = 0.f;
	static float OriginRotX = 0.f;

	// 줌이나 사격 동안은 둘러보기가 안된다.
	if (!m_PlayerScript->IsShot() && !(m_CameraFlag & SHOULDER))
	{
		if (KEY_TAP(KEY::Z))
		{
			// 현재의 RotY을 기억해놓는다.
			OriginRotY = m_CameraRot.y;
			OriginRotX = m_CameraRot.x;
			m_CameraFlag |= SEARCH;
		}
		if (KEY_RELEASED(KEY::Z))
		{
			// 줌 혹은 사격 도중 Z키가 눌려서 생기는 버그를 방지한다.
			if (m_CameraFlag & SEARCH)
			{
				m_CameraFlag |= SEARCH_RECOVER;
			}

			m_CameraFlag &= ~SEARCH;
		}
	}


	if (!(m_CameraFlag & SEARCH))
	{
		// 회복중이 아닌 일반시
		if (!(m_CameraFlag & SEARCH_RECOVER))
		{
			m_CameraRot.y = m_PlayerRot.y + 180.f;
		}
		// 회복중일때 저장해놓은 위치로 카메라를 원위치 시킨다.
		else
		{
			float RecoverSpeed = 10.f;

			m_CameraRot.y = FloatLerp(m_CameraRot.y, OriginRotY, RecoverSpeed);
			m_CameraRot.x = FloatLerp(m_CameraRot.x, OriginRotX, RecoverSpeed);

			// 특정 구간에 도달하면 회복을 종료시킨다.
			if (fabs(m_CameraRot.y - OriginRotY) < 1.f && fabs(m_CameraRot.x - OriginRotX) < 1.f)
			{
				m_CameraFlag &= ~SEARCH_RECOVER;
			}
		}
	}
}

void CameraController::UpdateShoulderRecover()
{
	// 기존 카메라 위치로 회복시켜준다.
	if (m_AdjustFinalDistance < m_AdjustNormalDistance)
	{
		m_AdjustFinalDistance = FloatLerp(m_AdjustFinalDistance, m_AdjustNormalDistance, 50.f);
	}
	if (m_AdjustFinalHeight < m_AdjustNormalHeight)
	{
		m_AdjustFinalHeight = FloatLerp(m_AdjustFinalHeight, m_AdjustNormalHeight, 50.f);
	}
	if (abs(m_AdjustFinalHeight - m_AdjustNormalHeight) < 5.f && abs(m_AdjustFinalDistance - m_AdjustNormalDistance) < 5.f)
	{
		m_CameraFlag &= ~SHOULDER_RECOVER;
	}
}


void CameraController::UpdateTPSLean()
{
	// 왼쪽 Lean
	if (KEY_PRESSED(KEY::Q))
	{
		m_LateralOffset = FloatLerp(m_LateralOffset, -600.f, 10.f);
	}
	// 오른쪽 Lean
	else if (KEY_PRESSED(KEY::E))
	{
		m_LateralOffset = FloatLerp(m_LateralOffset, 1000.f, 10.f);
	}

	// 회복 (Q,E키가 안 눌린 상태면)
	else
	{
		m_LateralOffset = FloatLerp(m_LateralOffset, 300.f, 10.f);
	}
}

void CameraController::UpdateFPSLean()
{
	static float leanAngle = 0.f;
	static float leanOffset = 0.f;

	// 왼쪽 Lean
	if (KEY_PRESSED(KEY::Q))
	{
		leanAngle = FloatLerp(leanAngle, 10.f, 10.f);
		leanOffset = FloatLerp(leanOffset, 30.f, 10.f);
	}
	// 오른쪽 Lean
	else if (KEY_PRESSED(KEY::E))
	{
		leanAngle = FloatLerp(leanAngle, -10.f, 10.f);
		leanOffset = FloatLerp(leanOffset, -30.f, 10.f);
	}

	// 회복 (Q,E키가 안 눌린 상태면)
	else
	{
		leanAngle = FloatLerp(leanAngle, 0.f, 10.f);
		leanOffset = FloatLerp(leanOffset, 0.f, 10.f);
	}

	m_CameraRot.z = leanAngle;

	m_CameraPos.x += leanOffset;
	m_CameraPos.y += leanOffset;
	m_CameraPos.z += leanOffset;
}


void CameraController::ChangePS(bool _IsTps)
{
	// TPS로 전환
	if (_IsTps)
	{
		m_CameraFlag |= TPS;
		m_CameraFlag &= ~WAS_TPS;
	}
	// FPS로 전환
	else
	{
		m_CameraFlag &= ~TPS;
	}
	m_InventoryScript->ConvertPS();
	GetOwner()->Camera()->LayerCheck(3);
	GetOwner()->Camera()->LayerCheck(4);
}


void CameraController::SaveComponent(FILE* _File)
{
	fwrite(&m_CameraSpeed, sizeof(float), 1, _File);
}

void CameraController::LoadComponent(FILE* _File)
{
	fread(&m_CameraSpeed, sizeof(float), 1, _File);
}

void CameraController::SetFlag(CAM_FLAG _flag, bool _value)
{
	if (_value)
		m_CameraFlag |= _flag;
	else
		m_CameraFlag &= ~_flag;
}
