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
	: CScript(static_cast<UINT>(SCRIPT_TYPE::CAMERASCRIPT))
	, m_Player(nullptr)
	, m_CameraSpeed(100.f)
	, m_bSearch(false)
	, m_bSearchRecover(false)
	, m_bRight(true)
	, m_bShoulderRecover(false)
	, m_bShoulder(false)
	, m_bADS(false)
	, m_bChangeFocus(false)
	, m_bTPS(true)
	, m_bWasTPS(false)
	, m_bCliked_First(false)
	, m_RecoilTime(0.f)
	, m_RecoilAmount_vertical(0.f)
	, m_RecoilAmount_horizontal(0.f)
{
}

CameraController::~CameraController()
{
}

void CameraController::Begin()
{
	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
	m_PlayerScript = static_cast<PlayerCharacter*>(m_Player->GetScript(PLAYERSCRIPT));
	m_InventoryScript = static_cast<InventoryController*>(m_Player->GetScript(INVENTORYSCRIPT));
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

	Vec3 vCameraPos = Transform()->GetRelativePos();
	Vec3 vCameraRot = Transform()->GetRelativeRotation();
	Vec3 vPlayerPos = m_Player->Transform()->GetRelativePos();
	Vec3 vPlayerRot = m_Player->Transform()->GetRelativeRotation();

	// 카메라 전환
	if (KEY_TAP(KEY::V))
	{
		if (!m_bShoulder && !m_bADS)
		{
			if (m_bTPS)
			{
				// TPS 비활성화, FPS 활성화
				m_bTPS = false;
				GetOwner()->Camera()->LayerCheck(3);
				GetOwner()->Camera()->LayerCheck(4);
			}

			else
			{
				// FPS 비활성화, TPS 활성화
				m_bTPS = true;
				GetOwner()->Camera()->LayerCheck(3);
				GetOwner()->Camera()->LayerCheck(4);
			}
		}
	}


	// TPS
	if (m_bTPS)
	{

		float RadianPitch = vCameraRot.x * XM_PI / 180.f;
		float DistanceRatio = 1000.f;
		float HeightRatio = 2000.f;

		static float lateralOffset = 300.f;
		static float adjustNormalDistance = 0.f;
		static float adjustNormalHeight = 0.f;
		static float adjustFinalDistance = 0.f;
		static float adjustFinalHeight = 0.f;


		// 평상시에 카메라 있을 위치를 계산해 놓는다.
		adjustNormalDistance = DistanceRatio * cosf(RadianPitch);
		adjustNormalHeight = HeightRatio + DistanceRatio * sinf(RadianPitch);

		// Player가 앞을 보고 있으면 Dir는 - , 아니면 +
		float radian_PlayerRotY = vPlayerRot.y * XM_PI / 180.f;

		// Player의 후방 벡터
		Vec3 vBackDir = { sinf(radian_PlayerRotY), 0.f, cosf(radian_PlayerRotY) };

		// Player의 오른쪽 벡터
		Vec3 vRightDir = { -cosf(radian_PlayerRotY), 0.f, sinf(radian_PlayerRotY) };



		static float OriginDistance = 0.f;
		static float OriginHeight = 0.f;
		static float ObjectiveDistance = 0.f;
		static float ObjectiveHeight = 0.f;
		static float ObjectiveLateralOff = 0.f;

		// 평상시 상태에서 위치 값 
		if (!m_bShoulder)
		{
			adjustFinalDistance = adjustNormalDistance;
			adjustFinalHeight = adjustNormalHeight;
		}
		// 견착상태에서 카메라가 향할 위치를 계산해놓는다.
		else
		{
			ObjectiveDistance = adjustNormalDistance * 0.5f;
			ObjectiveHeight = adjustNormalHeight * 0.9f;

			adjustFinalDistance = adjustFinalDistance + (ObjectiveDistance - adjustFinalDistance) * 50.f * DT;
			adjustFinalHeight = adjustFinalHeight + (ObjectiveHeight - adjustFinalHeight) * 50.f * DT;
		}

		//
		// 견착과 줌
		//

		// 줌 회복 상태가 아니거나 둘러보기 상태가 아니라면 줌을 활성화 한다.
		if (!m_bShoulderRecover && !m_bSearch && iWeaponIdx <= SECONDARY_FIRST)
		{
			
			if (KEY_TAP(KEY::RBTN))
			{
				m_bCliked_First = true;
			}
			if (KEY_PRESSED(KEY::RBTN))
			{
				m_AccTime += DT;

				// 특정 시간 이상 누른상태라면 견착모드로 진입
				if (0.1f < m_AccTime)
				{
					m_bShoulder = true;
				}
			}
			if (KEY_RELEASED(KEY::RBTN))
			{
				m_AccTime = 0.f;
				// 특정 시간 이하, 즉 우클릭을 클릭만 했을때는 정조준 진입
				if (!m_bShoulder && !m_bWasTPS)
				{
					m_bADS = true;
				}
				else if (m_bWasTPS)
				{
					m_bWasTPS = false;
				}
			}

			// 견착 상태 
			if (m_bShoulder)
			{
				if (m_bCliked_First)
				{
					lateralOffset = 500.f;
					m_bCliked_First = false;
				}
				

				// 카메라위 위치를 조정하여 줌해준다.
				if (KEY_PRESSED(KEY::RBTN))
				{
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
					m_bShoulderRecover = true;
					m_bShoulder = false;
					lateralOffset = 300.f;
				}
			}

			// 줌 상태
			if (m_bADS)
			{
				// TPS 비활성화, FPS 활성화
				m_bTPS = false;
				m_bWasTPS = true;
				GetOwner()->Camera()->LayerCheck(3);
				GetOwner()->Camera()->LayerCheck(4);			
			}
		}

		// 줌 회복 
		else
		{
			// 기존 카메라 위치로 회복시켜준다.
			if (adjustFinalDistance < adjustNormalDistance)
			{
				adjustFinalDistance = adjustFinalDistance + (adjustNormalDistance - adjustFinalDistance) * 50.f * DT;
			}
			if (adjustFinalHeight < adjustNormalHeight)
			{
				adjustFinalHeight = adjustFinalHeight + (adjustNormalHeight - adjustFinalHeight) * 50.f * DT;
			}
			if (abs(adjustFinalHeight - adjustNormalHeight) < 5.f && abs(adjustFinalDistance - adjustNormalDistance) < 5.f)
			{
				m_bShoulderRecover = false;
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
		vCameraPos.x = vPlayerPos.x + adjustFinalDistance * vBackDir.x;
		vCameraPos.y = vPlayerPos.y + adjustFinalHeight;
		vCameraPos.z = vPlayerPos.z + adjustFinalDistance * vBackDir.z;


		//  Player가 Camera상에서 약간 왼쪽에 위치하게끔 보정한다.
		vCameraPos.x += lateralOffset * vRightDir.x;
		vCameraPos.z += lateralOffset * vRightDir.z;

		//
		// Search
		//
		static float OriginRotY = 0.f;
		static float OriginRotX = 0.f;

		// 줌이나 사격 동안은 둘러보기가 안된다.
		if (!m_PlayerScript->IsShot() && !m_bShoulder)
		{
			if (KEY_TAP(KEY::Z))
			{
				// 현재의 RotY을 기억해놓는다.
				OriginRotY = vCameraRot.y;
				OriginRotX = vCameraRot.x;
				m_bSearch = true;
			}
			if (KEY_RELEASED(KEY::Z))
			{
				// 줌 혹은 사격 도중 Z키가 눌려서 생기는 버그를 방지한다.
				if (m_bSearch)
				{
					m_bSearchRecover = true;
				}
				m_bSearch = false;
			}
		}


		if (!m_bSearch)
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
				if (fabs(vCameraRot.y - OriginRotY) < 1.f && fabs(vCameraRot.x - OriginRotX) < 1.f)
				{
					m_bSearchRecover = false;
				}
			}
		}
	}

	// FPS
	else
	{
		// 견착모드였다면 해제한다.
		m_bShoulder = false;

		// 카메라의 위치를 Player의 위치로 바꿔준다.
		vCameraPos.x = vPlayerPos.x;
		vCameraPos.y = vPlayerPos.y + 1800.f;
		vCameraPos.z = vPlayerPos.z;

		vCameraRot.y = vPlayerRot.y + 180.f;
		Transform()->SetRelativeRotation(vCameraRot);

		// 줌 활성화
		if (iWeaponIdx <= SECONDARY_FIRST)
		{
			if (KEY_TAP(KEY::RBTN))
			{
				// TPS에서 줌으로 넘어온 경우 줌을 풀때 TPS로 넘어간다.
				if (m_bWasTPS && m_bADS)
				{
					m_bTPS = true;
					GetOwner()->Camera()->LayerCheck(3);
					GetOwner()->Camera()->LayerCheck(4);
				}
				m_bADS == true ? m_bADS = false : m_bADS = true;
			}
		}

	}
	



	//
	// 반동
	//
	
	// 플레이어가 총기를 발사하는 중이라면
	if (m_PlayerScript->IsShot())
	{
		m_RecoilTime += DT;

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
			

			vCameraRot.x -= m_RecoilAmount_vertical;
			vPlayerRot.y -= m_RecoilAmount_horizontal;
			if (vCameraRot.x < -90.f)
			{
				vCameraRot.x = -90.f;
			}

			Transform()->SetRelativeRotation(vCameraRot);
			m_Player->Transform()->SetRelativeRotation(vPlayerRot);
		}
	}


	Transform()->SetRelativePos(vCameraPos);
}

void CameraController::SaveComponent(FILE* _File)
{
	fwrite(&m_CameraSpeed, sizeof(float), 1, _File);
}

void CameraController::LoadComponent(FILE* _File)
{
	fread(&m_CameraSpeed, sizeof(float), 1, _File);
}
