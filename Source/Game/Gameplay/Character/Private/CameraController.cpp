#include "pch.h"
#include "Game/Gameplay/Character/Public/CameraController.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Physics/RayCollider.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/SoundManager.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/GamePlay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"

CameraController::CameraController()
	: CScript(SCRIPT_TYPE::CAMERASCRIPT)
	, m_Player(nullptr)
	, m_PlayerScript(nullptr)
	, m_InventoryScript(nullptr)
	, m_CameraPos(Vec3(0.f, 0.f, 0.f))
	, m_CameraRot(Vec3(0.f, 0.f, 0.f))
	, m_PlayerPos(Vec3(0.f, 0.f, 0.f))
	, m_PlayerRot(Vec3(0.f, 0.f, 0.f))
	, m_ObstacleAdjustPos(Vec3(0.f, 0.f, 0.f))
	, m_ObstaclePos(Vec3(0.f, 0.f, 0.f))
	, m_ObstacleScale(Vec3(0.f, 0.f, 0.f))
	, m_OriginDistance(0.f)
	, m_RayDistance(0.f)
	, m_CameraSpeed(500.f)
	, m_CameraYOffset(0.f)
	, m_CurClipAccTime(0.f)
	, m_RecoilTime(0.f)
	, m_ObstacleResetTime(0.f)
	, m_RecoilAmount_vertical(0.f)
	, m_RecoilAmount_horizontal(0.f)
	, m_TargetRecoilRotX(0.f)
	, m_TargetRecoilRotY(0.f)
	, m_LateralOffset(300.f)
	, m_ObjectiveLateralOff(0.f)
	, m_AdjustNormalDistance(0.f)
	, m_AdjustNormalHeight(0.f)
	, m_AdjustFinalDistance(0.f)
	, m_AdjustFinalHeight(0.f)
	, m_ObjectiveShoulderDistance(0.f)
	, m_ObjectiveShoulderHeight(0.f)
	, m_LastVerticalVelocity(0.f)
	, m_VerticalSmoothTime(0.15f) // 150ms의 스무딩 시간
	, m_CurrentVerticalVelocity(0.f)
	, m_CameraFlag(0)
	, m_CurStep(0)
	, m_CurSingleStep(0)
	, m_StepTimer(0.f)
	, m_SingleTimer(0.f)
	, m_bLevelChanged(false)
	, m_SingleFire(false)
	, m_SingleTargetValid(false)
	, m_SingleTargetCameraRotX(0.f)
	, m_SingleTargetPlayerRotY(0.f)	
{
}

CameraController::~CameraController() = default;

void CameraController::Begin()
{
	// 기본 카메라 모드 설정
	m_CameraFlag |= TPS;

	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));
	m_InventoryScript = static_cast<InventoryController*>(GetScriptWithType(m_Player, SCRIPT_TYPE::INVENTORYSCRIPT));

	m_bLevelChanged = true;
}

void CameraController::Tick()
{
	if (KEY_TAP(KEY::F1))
	{
		if (GetOwner()->GetName() == L"Cam Ray")
		{
			if (!(m_CameraFlag & FREE_PS))
			{
				// 부모를 없는 독립 개체로 바꿔준다.
				AddChild(nullptr, GetOwner());
				Transform()->SetRelativePos(
					CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera")->Transform()->
					GetRelativePos());
				Transform()->SetRelativeRotation(
					CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera")->Transform()->
					GetRelativeRotation());
			}
			else
			{
				AddChild(CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera"), GetOwner());
				Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
				Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
			}
		}
		m_CameraFlag ^= FREE_PS;
	}

	if (GetOwner()->GetName() == L"Cam Ray")
		return;

	if (m_CameraFlag & FREE_PS)
	{
		CameraDebugMove();
	}
	else
	{
		if (ORTHOGRAPHIC == Camera()->GetProjType())
			CameraOrthgraphicMove();
		else
			CameraPerspectiveMove();
	}


	// 소리테스트 위치 업데이트
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vforward = Transform()->GetWorldDir(DIR_TYPE::FRONT);
	Vec3 vUp = Transform()->GetWorldDir(DIR_TYPE::UP);
	FSoundManager::GetInst()->UpdateListener(vPos, vforward, vUp);
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

	// LOG_DEBUG_F("[Camera][Player] 플레이어 위치: {}, {}, {}", m_PlayerPos.x, m_PlayerPos.y, m_PlayerPos.z);
	// LOG_DEBUG_F("[Camera][Player] 플레이어 회전: {}, {}, {}", m_PlayerRot.x, m_PlayerRot.y, m_PlayerRot.z);
	// LOG_DEBUG_F("[Camera][Player] 카메라 위치: {}, {}, {}", m_CameraPos.x, m_CameraPos.y, m_CameraPos.z);
	// LOG_DEBUG_F("[Camera][Player] 카메라 회전: {}, {}, {}", m_CameraRot.x, m_CameraRot.y, m_CameraRot.z);


	Vec3 vDirtoPlayer = m_PlayerPos - m_CameraPos;
	vDirtoPlayer.Normalize();

	RayCollider()->SetFinalDirection(vDirtoPlayer);

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
		UpdateStance();

		// 줌 회복 상태가 아니거나 둘러보기 상태가 아니라면 줌을 활성화 한다.
		if (!(m_CameraFlag & SHOULDER_RECOVER) && !(m_CameraFlag & SEARCH) && iWeaponIdx <= SECONDARY_FIRST)
		{
			// 우클릭 동작 수행 (줌과 견착)
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
		else if (m_CameraFlag & SHOULDER_RECOVER)
		{
			UpdateShoulderRecover();
		}

		if (!(m_CameraFlag & SHOULDER))
		{
			UpdateTPSLean();
		}

		// Player 위치에 보정값을 더해 위치를 고정시킨다.
		m_CameraPos.x = m_PlayerPos.x + m_AdjustFinalDistance * m_Player->Transform()->GetWorldDir(DIR_TYPE::FRONT).x;
		m_CameraPos.y = m_PlayerPos.y + m_AdjustFinalHeight + m_CameraYOffset;
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

		UpdateStance();

		// 카메라의 위치를 Player의 위치로 바꿔준다.
		m_CameraPos.x = m_PlayerPos.x;
		m_CameraPos.y = m_PlayerPos.y + 1800.f + m_CameraYOffset;
		m_CameraPos.z = m_PlayerPos.z;

		m_CameraRot.y = m_PlayerRot.y + 180.f;


		UpdateFPSLean();


		// 줌 활성화
		if (iWeaponIdx <= SECONDARY_FIRST)
		{
			if (KEY_TAP(KEY::RBTN) && !m_PlayerScript->IsInventoryOpened())
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
	// 플레이어가 총기를 발사하는 중이라면
	if (m_PlayerScript->GetStateEnum() == PLAYER_STATE::Player_Gun_Fire)
	{
		UpdateRecoil();
	}
	else
	{
		ResetRecoilPattern();
	}

	if (m_CameraFlag & SINGLE_RECOIL_UPDATE)
	{
		UpdateSingleRecoil();
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
		fTimes = 0.95f;
		float fDestFOV = (XM_PI / 2.f) * fTimes;

		float fChangeSpeed = 300.f;

		fCurFOV = MoveToward(fCurFOV, fDestFOV, fChangeSpeed);
		//fCurFOV + (fDestFOV - fCurFOV) * fChangeSpeed * DT;
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

		float fChangeSpeed = 300.f;

		fCurFOV = MoveToward(fCurFOV, fDestFOV, fChangeSpeed);
		//fCurFOV + (fDestFOV - fCurFOV) * fChangeSpeed * DT;
		GetOwner()->Camera()->SetFOV(fCurFOV);

		// 특정 구간에 도달하면 변환을 종료시킨다.
		if (fabs(fCurFOV - fDestFOV) < 0.01f && fabs(fCurFOV - fDestFOV) < 0.01f)
		{
			GetOwner()->Camera()->SetFOV(fDestFOV);
			m_CameraFlag &= ~CHANGE_FOV;
		}
	}
}


void CameraController::UpdateRecoil()
{
	GunController* pGunScript = static_cast<GunController*>(m_InventoryScript->GetCurWeaponController());
	if (!pGunScript->IsFire() || !pGunScript)
		return;

	vector<RecoilPatternStep> vecPattern = pGunScript->GetRecoilPattern();
	if (vecPattern.empty())
		return;

	m_CameraFlag |= RECOIL_UPDATE;

	// Step 진행
	m_StepTimer += DT;
	const auto& step = vecPattern[m_CurStep];

	// Step Time 이 지나면 다음으로
	if (step.time <= m_StepTimer)
	{
		m_StepTimer -= step.time;
		m_CurStep++;
		m_StepTimer = 0.f;
		if (vecPattern.size() <= m_CurStep)
		{
			m_CurStep = 0;
			m_CameraFlag &= ~RECOIL_UPDATE;
			return;
		}
	}

	// 랜덤한 노이즈값을 반동에 더해준다
	float pitch = ApplyRecoilNoise(step.pitch, 0.25f);
	float yaw = ApplyRecoilNoise(step.yaw, 0.5f);

	float pitchOffset = 0.f;
	float yawOffset = 0.f;

	float singlePower = pGunScript->GetSingleRecoilPower();
	float autoPower = 1.5f;
	float lerpSpeed = 10.f;

	// 최종 반동 목표 위치값을 구해준다
	if (pGunScript->IsAuto())
	{
		pitchOffset = pitch * autoPower;// *(DT / step.time);
		yawOffset = yaw * autoPower;// *(DT / step.time);

		m_CameraRot.x = FloatLerp(m_CameraRot.x, m_CameraRot.x - pitchOffset, lerpSpeed);
		m_PlayerRot.y = FloatLerp(m_PlayerRot.y, m_PlayerRot.y - yawOffset, lerpSpeed);
	}
	else
	{
		// 단발이면 한 번에 목표값 적용
		pitchOffset = pitch * singlePower;
		yawOffset = yaw * singlePower;

		m_CameraRot.x -= pitchOffset;
		m_PlayerRot.y -= yawOffset;
	}


	Transform()->SetRelativeRotation(m_CameraRot);
	m_Player->Transform()->SetRelativeRotation(m_PlayerRot);
}

void CameraController::UpdateSingleRecoil()
{
	GunController* pGunScript = static_cast<GunController*>(m_InventoryScript->GetCurWeaponController());
	if (!pGunScript)
	{
		m_CameraFlag &= ~SINGLE_RECOIL_UPDATE;
		return;
	}

	const vector<RecoilPatternStep> vecPattern = pGunScript->GetRecoilPattern();
	if (vecPattern.empty())
	{
		m_CameraFlag &= ~SINGLE_RECOIL_UPDATE;
		return;
	}

	// Step 진행
	const auto& step = vecPattern[m_CurSingleStep];

	// 타이머 갱신
	m_SingleTimer += DT;

	// 만약 플레이어가 방금 발사했다면 (한 발 들어온 프레임)
	if (m_SingleFire)
	{
		// 랜덤 노이즈 + 파워
		float pitch = ApplyRecoilNoise(step.pitch, 0.25f);
		float yaw = ApplyRecoilNoise(step.yaw, 0.5f);
		float singlePower = pGunScript->GetSingleRecoilPower();

		float pitchOffset = pitch * singlePower;
		float yawOffset = yaw * singlePower;

		// 현재 카메라,플레이어 회전에서 목표 회전 저장
		m_SingleTargetCameraRotX = m_CameraRot.x; // 복사
		m_SingleTargetPlayerRotY = m_PlayerRot.y;

		m_SingleTargetCameraRotX -= pitchOffset;
		m_SingleTargetPlayerRotY -= yawOffset;

		m_SingleTargetValid = true;

		// 발사 신호 소비 (다음 발사가 들어올 때 다시 true로 설정됨)
		m_SingleFire = false;

		// 연속 단발(다음 샷이 0.5초 이내 들어오면 step++)
		if (m_SingleTimer < 0.5f)
		{
			m_CurSingleStep++;
			if (vecPattern.size() <= m_CurSingleStep)
			{
				m_CurSingleStep = 0;
			}
		}
		m_SingleTimer = 0.f;
	}

	// 만약 Targer이 없으면 초기화
	if (!m_SingleTargetValid)
	{
		// target이 없고 타이머가 길어지면 해제
		if (m_SingleTimer > 0.5f)
		{
			m_CurSingleStep = 0;
			m_SingleTimer = 0.f;
			m_CameraFlag &= ~SINGLE_RECOIL_UPDATE;
		}
		return;
	}

	float lerpSpeed = 10.0f; // 튜닝
	// lerp로 적용 (프레임 독립적)
	m_CameraRot.x = FloatLerp(m_CameraRot.x, m_SingleTargetCameraRotX, lerpSpeed);
	m_PlayerRot.y = FloatLerp(m_PlayerRot.y, m_SingleTargetPlayerRotY, lerpSpeed);

	
	Transform()->SetRelativeRotation(m_CameraRot);
	m_Player->Transform()->SetRelativeRotation(m_PlayerRot);

	const float threshold = 0.001f; // 각도 단위에 따라 조절
	bool reachedPitch = fabsf(m_CameraRot.x - m_SingleTargetCameraRotX) < threshold;
	bool reachedYaw = fabsf(m_PlayerRot.y - m_SingleTargetPlayerRotY) < threshold;

	// 목표에 도달 또는 타임 아웃
	if ((reachedPitch && reachedYaw) || (m_SingleTimer > 0.4f))
	{
		m_SingleTargetValid = false;
		m_CurSingleStep = 0;
		m_SingleTimer = 0.f;
		m_CameraFlag &= ~SINGLE_RECOIL_UPDATE;
	}
}

float CameraController::ApplyRecoilNoise(float _value, float _noiseScale)
{
	float noise = ((rand() % 200) - 100) / 100.f; // -1.0 ~ +1.0
	return _value + _value * noise * _noiseScale;
}

void CameraController::ResetRecoilPattern()
{
	m_CurStep = 0;
	m_StepTimer = 0.f;
	m_CameraFlag &= ~RECOIL_UPDATE;
}

void CameraController::UpdateTPSCameraAdjustments()
{
	float RadianPitch = m_CameraRot.x * XM_PI / 180.f;
	float DistanceRatio = 1000.f;
	float HeightRatio = 1700.f;

	// 평상시에 카메라 있을 위치를 계산해 놓는다.
	m_AdjustNormalDistance = DistanceRatio * cosf(RadianPitch);
	m_AdjustNormalHeight = HeightRatio + DistanceRatio * sinf(RadianPitch);

	float targetDistance = m_AdjustNormalDistance;
	float targetHeight = m_AdjustNormalHeight;

	// 견착시 목표값
	if (m_CameraFlag & SHOULDER)
	{
		targetDistance *= 0.5f;
		targetHeight *= 0.9f;
	}

	// 장애물 충돌시 목표값
	if (m_CameraFlag & OBSTACLE_DETECT)
	{
		float obstacleDist = 0.f;
		float corValue = 0.f;
		Vec3 Dir = m_PlayerPos - m_ObstaclePos;
		Dir.Normalize();

		if (abs(Dir.z) > abs(Dir.x))
		{
			if (Dir.z > 0.5f)
			{
				m_ObstacleAdjustPos.z = m_ObstaclePos.z + m_ObstacleScale.z / 2.f + corValue;
				obstacleDist = m_PlayerPos.z - m_ObstacleAdjustPos.z;
			}
			if (Dir.z < -0.5f)
			{
				m_ObstacleAdjustPos.z = m_ObstaclePos.z - m_ObstacleScale.z / 2.f - corValue;
				obstacleDist = m_PlayerPos.z - m_ObstacleAdjustPos.z;
			}
		}

		if (abs(Dir.z) < abs(Dir.x))
		{
			if (Dir.x > 0.5f)
			{
				m_ObstacleAdjustPos.x = m_ObstaclePos.x + m_ObstacleScale.x / 2.f + corValue;
				obstacleDist = m_PlayerPos.x - m_ObstacleAdjustPos.x;
			}
			if (Dir.x < -0.5f)
			{
				m_ObstacleAdjustPos.x = m_ObstaclePos.x - m_ObstacleScale.x / 2.f - corValue;
				obstacleDist = m_PlayerPos.x - m_ObstacleAdjustPos.x;
			}
		}

		m_OriginDistance = targetDistance;
		targetDistance = min(targetDistance, obstacleDist);
	}

	// Add Smoothing
	constexpr float SmoothSpeed = 500.0f;
	constexpr float VerticalDamping = 0.7f;

	// 평상시
	m_AdjustFinalDistance = MoveToward(m_AdjustFinalDistance, targetDistance, SmoothSpeed);
	if (abs(m_AdjustFinalDistance - targetDistance) < 10.f)
	{
		m_AdjustFinalDistance = targetDistance;
	}


	// Vertical Smoothing
	float heightDiff = targetHeight - m_AdjustFinalHeight;
	m_AdjustFinalHeight += heightDiff * VerticalDamping * SmoothSpeed * DT;

	// Add Clamping
	const float maxHeightChange = 50.0f * DT;
	m_AdjustFinalHeight = std::clamp(m_AdjustFinalHeight,
		targetHeight - maxHeightChange,
		targetHeight + maxHeightChange);

	// 레벨이 시작되거나 바뀌어서 플레이어의 위치가 큰값으로 변동됨 -> 고정으로 카메라를 바로 붙혀준다.
	if (m_bLevelChanged)
	{
		m_AdjustFinalDistance = targetDistance;
		m_AdjustFinalHeight = targetHeight;

		m_LevelChangeTime += DT;
		// 값들이 세팅되는 시간을 기다려야 함. (다른 방법이 있나?)
		if (1.f < m_LevelChangeTime)
		{
			m_bLevelChanged = false;
			m_LevelChangeTime = 0.f;
		}
	}

	// TPS 충돌 보정 코드 (보류)
		// 충돌 콜백이 끊김 -> 클리어 검증
		// if (m_CameraFlag & OBSTACLE_CLEAR_PENDING)
		// {
		// 	m_ObstacleResetTime += DT;
		// 	if (2.f < m_ObstacleResetTime)
		// 	{
		// 		// 이제 진짜 복구 단계로 전환
		// 		m_CameraFlag &= ~OBSTACLE_CLEAR_PENDING;
		// 		m_CameraFlag |= OBSTACLE_DETECT_END;
		// 		// 복구 시작 거리를 현재 보정된 최종값으로 초기화
		// 		m_RayDistance = m_AdjustFinalDistance;
		// 	}
		// }
		//
		// // 복구 단계
		// if (m_CameraFlag & OBSTACLE_DETECT_END)
		// {
		// 	RayCollider()->SetOffset(Vec3(0, 0, -m_RayDistance));
		// 	m_RayDistance += 500.f * DT;
		//
		// 	// 원래 거리까지 도달했거나 약간 넘어섰을 때
		// 	if (abs(m_RayDistance) >= abs(m_OriginDistance))
		// 	{
		// 		// 콜백으로도 다시 Overlap이 안 들어온 상태이므로
		// 		RayCollider()->SetOffset(Vec3(0, 0, 0));
		// 		m_CameraFlag &= ~OBSTACLE_DETECT_END;
		// 		m_ObstacleResetTime = 0.f;
		// 	}
		// }
	//
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
			m_CurClipAccTime += DT;

			// 특정 시간 이상 누른상태라면 견착모드로 진입
			if (0.1f < m_CurClipAccTime)
			{
				m_CameraFlag |= SHOULDER;
			}
		}
		if (KEY_RELEASED(KEY::RBTN))
		{
			m_CurClipAccTime = 0.f;
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
		m_LateralOffset = MoveToward(m_LateralOffset, m_ObjectiveLateralOff, 300.f);
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
	if (m_PlayerScript->GetStateEnum() != PLAYER_STATE::Player_Gun_Fire && !(m_CameraFlag & SHOULDER) && !(m_CameraFlag & SEARCH_RECOVER))
	{
		if (KEY_TAP(KEY::LCTRL))
		{
			// 현재의 RotY을 기억해놓는다.
			OriginRotY = m_CameraRot.y;
			OriginRotX = m_CameraRot.x;
			m_CameraFlag |= SEARCH;
		}
		if (KEY_RELEASED(KEY::LCTRL))
		{
			// 줌 혹은 사격 도중 CTRL키가 눌려서 생기는 버그를 방지한다.
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
			float RecoverSpeed = 200.f;

			m_CameraRot.y = MoveToward(m_CameraRot.y, OriginRotY, RecoverSpeed);
			m_CameraRot.x = MoveToward(m_CameraRot.x, OriginRotX, RecoverSpeed);

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
		m_AdjustFinalDistance = MoveToward(m_AdjustFinalDistance, m_AdjustNormalDistance, 300.f);
	}
	if (m_AdjustFinalHeight < m_AdjustNormalHeight)
	{
		m_AdjustFinalHeight = MoveToward(m_AdjustFinalHeight, m_AdjustNormalHeight, 300.f);
	}
	if (abs(m_AdjustFinalHeight - m_AdjustNormalHeight) < 5.f
		&& abs(m_AdjustFinalDistance - m_AdjustNormalDistance) < 5.f)
	{
		m_CameraFlag &= ~SHOULDER_RECOVER;
	}
}


void CameraController::UpdateTPSLean()
{
	if (!(m_CameraFlag & LAYING))
	{
		// 왼쪽 Lean
		if (KEY_PRESSED(KEY::Q))
		{
			m_LateralOffset = MoveToward(m_LateralOffset, -600.f, 300.f);
		}
		// 오른쪽 Lean
		else if (KEY_PRESSED(KEY::E))
		{
			m_LateralOffset = MoveToward(m_LateralOffset, 1000.f, 300.f);
		}
		// 회복 (Q,E키가 안 눌린 상태면)
		else
		{
			m_LateralOffset = MoveToward(m_LateralOffset, 300.f, 300.f);
		}
	}
	else
	{
		m_LateralOffset = 300.f;
	}
}

void CameraController::UpdateFPSLean()
{
	static float leanAngle = 0.f;
	static float leanOffset = 0.f;

	if (!(m_CameraFlag & LAYING))
	{
		// 왼쪽 Lean
		if (KEY_PRESSED(KEY::Q))
		{
			leanAngle = MoveToward(leanAngle, 10.f, 300.f);
			leanOffset = MoveToward(leanOffset, 30.f, 300.f);
		}
		// 오른쪽 Lean
		else if (KEY_PRESSED(KEY::E))
		{
			leanAngle = MoveToward(leanAngle, -10.f, 300.f);
			leanOffset = MoveToward(leanOffset, -30.f, 300.f);
		}

		// 회복 (Q,E키가 안 눌린 상태면)
		else
		{
			leanAngle = MoveToward(leanAngle, 0.f, 300.f);
			leanOffset = MoveToward(leanOffset, 0.f, 300.f);
		}
	}
	else
	{
		leanAngle = 0.f;
		leanOffset = 0.f;
	}

	m_CameraRot.z = leanAngle;

	m_CameraPos.x += leanOffset;
	m_CameraPos.y += leanOffset;
	m_CameraPos.z += leanOffset;
}


void CameraController::UpdateStance()
{
	static float DesPosY = 0.f;

	// C키를 통해 앉기, 일어서기 수행
	if (KEY_TAP(KEY::C))
	{
		// 누워있는 상태라면 해제시킨다.
		m_CameraFlag &= ~LAYING;

		m_CameraFlag |= CHANGE_STANCE;

		// 현재 앉아있는 상태라면 일어서야한다.
		if (m_CameraFlag & SITTING)
		{
			DesPosY = 0.f;
			m_PlayerScript->SetMotionState(MOTION_STATE::STAND);
		}
		else
		{
			DesPosY = -600.f;
			m_PlayerScript->SetMotionState(MOTION_STATE::CROUCH);
		}

		m_CameraFlag ^= SITTING;
	}

	if (KEY_TAP(KEY::Z))
	{
		// 앉아있는 상태라면 해제시킨다.
		m_CameraFlag &= ~SITTING;

		m_CameraFlag |= CHANGE_STANCE;

		// 현재 누워있는 상태라면 일어서야한다.
		if (m_CameraFlag & LAYING)
		{
			DesPosY = 0.f;
			m_PlayerScript->SetMotionState(MOTION_STATE::STAND);
		}
		else
		{
			DesPosY = -1200.f;
			m_PlayerScript->SetMotionState(MOTION_STATE::PRONE);
		}

		m_CameraFlag ^= LAYING;
	}

	if (m_CameraFlag & CHANGE_STANCE)
	{
		m_CameraYOffset = MoveToward(m_CameraYOffset, DesPosY, 100.f);

		if (abs(m_CameraYOffset - DesPosY) < 2.f)
		{
			m_CameraYOffset = DesPosY;
			m_CameraFlag &= ~CHANGE_STANCE;
		}
	}
}

void CameraController::ChangePS(bool _IsTps)
{
	// TPS로 전환
	if (_IsTps)
	{
		m_CameraFlag |= TPS;

		// Player Layer On
		GetOwner()->Camera()->LayerOn(3);
		GetOwner()->Camera()->LayerOff(4);
	}
	// FPS로 전환
	else
	{
		m_CameraFlag &= ~TPS;

		// Player Layer Off
		GetOwner()->Camera()->LayerOff(3);
		GetOwner()->Camera()->LayerOn(4);
	}

	// Common Logic
	m_InventoryScript->ConvertPS();
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

void CameraController::CameraDebugMove()
{
	float Speed = m_CameraSpeed;
	if (KEY_PRESSED(KEY::LSHIFT))
		Speed *= 5.f;

	Vec3 vFront = Transform()->GetLocalDir(DIR_TYPE::FRONT);
	Vec3 vRight = Transform()->GetLocalDir(DIR_TYPE::RIGHT);

	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
		vPos += vFront * EngineDT * Speed;
	if (KEY_PRESSED(KEY::S))
		vPos -= vFront * EngineDT * Speed;
	if (KEY_PRESSED(KEY::A))
		vPos -= vRight * EngineDT * Speed;
	if (KEY_PRESSED(KEY::D))
		vPos += vRight * EngineDT * Speed;

	Transform()->SetRelativePos(vPos);

	// 마우스 방향에 따른 오브젝트 회전
	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec3 vRot = Transform()->GetRelativeRotation();

		Vec2 vDir = CKeyMgr::GetInst()->GetMouseDir();
		vRot.y += vDir.x * EngineDT * 15.f;
		vRot.x += vDir.y * EngineDT * 10.f;

		Transform()->SetRelativeRotation(vRot);
	}
}


void CameraController::BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::RayCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		if (OtherObject->GetName() == L"DeathBox")
		{
			m_ObstacleResetTime = 0.f;
			m_CameraFlag |= OBSTACLE_DETECT;
			m_ObstaclePos = OtherObject->Transform()->GetRelativePos();
			m_ObstacleScale = OtherObject->BoxCollider()->GetScale();

			m_ObstacleAdjustPos = m_ObstaclePos;
			m_ObstacleAdjustPos.y = m_PlayerPos.y;
			m_CameraFlag &= ~OBSTACLE_DETECT_END;
			m_CameraFlag &= ~OBSTACLE_CLEAR_PENDING;
		}
		if (OtherObject->GetName() == L"Interaction Handler")
		{
			return;
		}
		if (OtherObject->GetName() == L"Cam Ray")
		{
			return;
		}
		if (OtherObject->GetName() == L"Player")
		{
			return;
		}
		if (OtherObject->GetName() == L"MainCamera")
		{
			return;
		}
	}
}

void CameraController::Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::RayCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		if (OtherObject->GetName() == L"DeathBox")
		{
			m_CameraFlag |= OBSTACLE_DETECT;
			m_CameraFlag &= ~OBSTACLE_CLEAR_PENDING;
			m_CameraFlag &= ~OBSTACLE_DETECT_END;
			m_ObstacleResetTime = 0.f;
		}
		if (OtherObject->GetName() == L"Interaction Handler")
		{
			return;
		}
		if (OtherObject->GetName() == L"Cam Ray")
		{
			return;
		}
		if (OtherObject->GetName() == L"Player")
		{
			return;
		}
		if (OtherObject->GetName() == L"MainCamera")
		{
			return;
		}
	}
}


void CameraController::EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::RayCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		if (OtherObject->GetName() == L"DeathBox")
		{
			m_CameraFlag |= OBSTACLE_CLEAR_PENDING;
			m_CameraFlag &= ~OBSTACLE_DETECT_END;
			m_ObstacleResetTime = 0.f;
		}
		if (OtherObject->GetName() == L"Interaction Handler")
		{
			return;
		}
		if (OtherObject->GetName() == L"Cam Ray")
		{
			return;
		}
		if (OtherObject->GetName() == L"Player")
		{
			return;
		}
		if (OtherObject->GetName() == L"MainCamera")
		{
			return;
		}
	}
}


