#include "pch.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/RayCollider.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/SoundManager.h"
#include "Engine/System/Public/Rendering/Device/CDevice.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/Runtime/Public/State/CState.h"

#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Character/Public/InteractionHandler.h"
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"
#include "Game/Gameplay/Weapon/Public/ThrowableController.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/UI/Public/KillinfoUIScript.h"
#include "Game/Gameplay/Inventory/Public/Item.h"
#include "Game/Gameplay/Character/Public/CameraEffect.h"
#include "Game/System/Public/CGameMgr.h"

PlayerCharacter::PlayerCharacter()
	: CScript(SCRIPT_TYPE::PLAYERSCRIPT)
	, m_HitSoundIdx(-1)
	, m_FootstepSoundIdx(-1)
	, m_RunFootstepSoundIdx(-1)
	, m_Force(0.f)
	, m_Velocity(0.f)
	, m_GravityVelocity(0.f)
	, m_Mass(3.f)
	, m_Friction(100.f)
	, m_MaxSpeed(100.f)
	, m_GravityAccel(4.f)
	, m_GravityMaxSpeed(8.f)
	, m_JumpPower(2.15f)
	, m_IsGround(true)
	, m_bLean(false)
	, m_MouseSensitivity(0.1f)
	, m_bHitSoundPlayed(false)
	, m_bFirstFootStep(true)
	, m_CollObject(nullptr)
	, m_HeadColl(nullptr)
	, m_HitSoundAccTime(0.f)
	, m_FootStepSoundAccTime(0.f)
	, m_InventoryScript(nullptr)
	, m_CurHP(100.f)
	, m_CurBoost(0.f)
	, m_HealType(ITEM_TYPE::END)
	, m_HealRemainTime(0.f)
	, m_HealTotalTime(0.f)
	, m_HealAmount(0.f)
	, m_BoostRemainTime(0.f)
	, m_BoostSpeed(1.f)
	, m_KillCounts(0)
	, m_MotionState(MOTION_STATE::STAND)
	, m_InventoryOpened(false)
	, m_PauseUIOpened(false)
	, m_OptionUIOpened(false)
	, m_bMouseActive(false)
	, m_StateAccTime(0.f)
{
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "Player Mass", &m_Mass });	// 질량
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "Friction", &m_Friction });	// 마찰계수
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "MaxSpeed", &m_MaxSpeed });	// 최고속도
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "GravityAccel", &m_GravityAccel });	// 중력 가속도
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "GravityMaxSpeed", &m_GravityMaxSpeed });	// 중력 최대속도
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "CurHP", &m_CurHP });
}

PlayerCharacter::~PlayerCharacter()
{
}

void PlayerCharacter::Begin()
{
	// Collider
	m_HeadColl = CLevelMgr::GetInst()->FindObjectByName(L"Player Head");

	// UI
	CGameObject* HPUI = CGameMgr::GetInst()->GetHPUI();
	Vec2 uiSize = HPUI->UI()->GetRectSize();
	HPUI->UIRender()->GetMaterial(0)->SetScalarParam(VEC2_0, Vec2(uiSize.x / uiSize.y, m_SemiMaxHP / m_MaxHP));	// 고정 값
	HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, m_CurHP / m_MaxHP);
	HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, m_CurBoost / m_MaxBoost);

	// Script
	m_InventoryScript = static_cast<InventoryController*>(GetScriptWithType(GetOwner(), SCRIPT_TYPE::INVENTORYSCRIPT));

	CGameObject* InteractionHandlerObj = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Interaction Handler");
	m_InteractionScript = static_cast<InteractionHandler*>(GetScriptWithType(InteractionHandlerObj, SCRIPT_TYPE::INTERACTION_HANDLER));

	// 마우스 끄기
	CKeyMgr::GetInst()->SetCursorVisible(false);

	// Sound Load
	LoadPlayerSounds();

	// State Machine Base Set
	StateMachine()->SetChange(L"Player_Idle");
}

void PlayerCharacter::Tick()
{
	// 여기서 조건을 모두 판정함.

	// =================
	// 항상 작동하는 로직
	// =================


	// 이동 로직
	//PlayerMove();

	// UI 관리
	PlayerControlUI();

	// Heal 수치 계산
	PlayerHeal();


	// ==========
	// 조건부 로직
	// ==========

	// 마우스가 켜진 상태에서 키입력 방지,
	if (!m_bMouseActive && !CGameMgr::GetInst()->GetCamScript()->GetFlag(FREE_PS))
	{
		// 행동 로직
		PlayerStance();
	}

	if (KEY_TAP(KEY::F1))
	{
		SetMouseActive(!m_bMouseActive);
	}

	// 인벤토리 UI를 켠 상태라면 다른 로직 block
	if (m_InventoryOpened)
	{

	}

	else
	{
		if (!m_bMouseActive)
		{
			PlayerView();
		}

		PlayerControlWeapon();
	}

	if (m_bHitSoundPlayed)
	{
		m_HitSoundAccTime += DT;

		if (2.f < m_HitSoundAccTime)
		{
			m_bHitSoundPlayed = false;
			m_HitSoundAccTime = 0.f;
		}
	}
}


void PlayerCharacter::PlayerView()
{
	CGameObject* MainCam = CGameMgr::GetInst()->GetMainCam();
	CameraController* pCamScript = CGameMgr::GetInst()->GetCamScript();

	bool bRecover = pCamScript->GetFlag(SEARCH_RECOVER);
	bool bSearch = pCamScript->GetFlag(SEARCH);
	bool bShoulder = pCamScript->GetFlag(SHOULDER);
	bool bADS = pCamScript->GetFlag(ADS);
	bool bTPS = pCamScript->GetFlag(TPS);

	// 마우스 위치를 구해온다
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();
	Vec3 vPlayerRot = Transform()->GetRelativeRotation();
	Vec3 vCameraRot = MainCam->Transform()->GetRelativeRotation();

	// LOG_DEBUG_F("[Player] 마우스: {}, {}", vMousePos.x, vMousePos.y);
	// LOG_DEBUG_F("[Player] 플레이어 회전: {}, {}, {}", vPlayerRot.x, vPlayerRot.y, vPlayerRot.z);
	// LOG_DEBUG_F("[Player] 카메라 회전: {}, {}, {}", vCameraRot.x, vCameraRot.y, vCameraRot.z);

	// 화면의 중앙을 구한다.
	static Vec2 vResoulution = CDevice::GetInst()->GetRenderResolution();
	static float centerX = vResoulution.x / 2;
	static float centerY = vResoulution.y / 2;

	// 현재 마우스위치와 화면 정중앙의 변화값을 구한다.
	float deltaX = vMousePos.x - centerX;
	float deltaY = vMousePos.y - centerY;

	// 미세한 움직임으로 인한 회전을 방지하기 위해 Deadzone을 추가한다.
	//int deadzone = 1;
	//if (abs(deltaX) < deadzone)
	//{
	//	deltaX = 0;
	//}
	//else
	//{
	//	deltaX = (deltaX > 0) ? (deltaX - deadzone) : (deltaX + deadzone);
	//}

	static float OriginRotY = 0.f;

	// 평상시 & TPS 상태일 때
	if (!bADS && !bShoulder && bTPS && GetStateEnum() != PLAYER_STATE::Player_Gun_Fire)
	{
		// Search
		if (KEY_TAP(KEY::LCTRL))
		{
			OriginRotY = vCameraRot.y;
		}


		if (KEY_PRESSED(KEY::LCTRL))
		{
			// 줌 하는 동안 둘러보기 키가 눌린경우를 방지한다.
			if (bSearch && !bRecover)
			{
				vCameraRot.y += deltaX * m_MouseSensitivity;
				if (120.f < vCameraRot.y - OriginRotY)
				{
					vCameraRot.y = OriginRotY + 120.f;
				}
				if (vCameraRot.y - OriginRotY < -120.f)
				{
					vCameraRot.y = OriginRotY - 120.f;
				}
			}
		}
		// 평상시의 회전값 적용
		if (!bRecover && !bSearch)
		{
			vPlayerRot.y += deltaX * m_MouseSensitivity;
		}

	}

	// 줌 혹은 사격중
	else
	{
		// 해당 변화값으로 Player 회전 적용
		if (!bRecover)
		{
			vPlayerRot.y += deltaX * m_MouseSensitivity;
		}
	}

	// 위아래 회전
	if (!bRecover)
	{
		vCameraRot.x += deltaY * m_MouseSensitivity;
		if (vCameraRot.x < -90.f)
		{
			vCameraRot.x = -90.f;
		}
		else if (80.f < vCameraRot.x)
		{
			vCameraRot.x = 80.f;
		}

		MainCam->Transform()->SetRelativeRotation(vCameraRot);
	}

	Transform()->SetRelativeRotation(vPlayerRot);


	// 마우스 위치를 중앙으로 다시 초기화한다.
	CKeyMgr::GetInst()->SetMousePosAsCenter();

	Vec3 RayDir;
	float CameraFollowAngle = vCameraRot.x * (XM_PI / 180.f);

	// 좌우 이동은 이미 Player가 바라보는 방향에 종속적, 상하 이동만 Camera Angle 연동
	RayDir.x = 0.f;
	RayDir.y = -sin(CameraFollowAngle);
	RayDir.z = -cos(CameraFollowAngle);
	RayCollider()->SetDirection(RayDir);
}

void PlayerCharacter::PlayerStance()
{
	CameraController* pCamScript = CGameMgr::GetInst()->GetCamScript();

	// 기울이기
	if (KEY_PRESSED(KEY::Q) && !pCamScript->GetFlag(LAYING))
	{
		// 현재 무기 슬롯이 총이라면
		if (m_InventoryScript->GetCurSlotIdx() <= SECONDARY_FIRST)
		{
			WeaponController* pGunScript = m_InventoryScript->GetCurWeaponController();
			pGunScript->SetCurKey(KEY::Q);
			pGunScript->SetCurKeyState(KEY_STATE::PRESSED);
		}

		// Head Coll 이동
		if (pCamScript->GetFlag(SITTING))
		{
			m_HeadColl->Transform()->SetRelativePos(Vec3(15.f, 110.f, 0.f));
		}
		else
		{
			m_HeadColl->Transform()->SetRelativePos(Vec3(15.f, 170.f, 0.f));
		}
		m_HeadColl->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, -20.f));
		m_bLean = true;
	}

	if (KEY_PRESSED(KEY::E) && !pCamScript->GetFlag(LAYING))
	{
		// 현재 무기 슬롯이 총이라면
		if (m_InventoryScript->GetCurSlotIdx() <= SECONDARY_FIRST)
		{
			WeaponController* pGunScript = m_InventoryScript->GetCurWeaponController();
			pGunScript->SetCurKey(KEY::E);
			pGunScript->SetCurKeyState(KEY_STATE::PRESSED);
		}


		// Head Coll 이동
		if (pCamScript->GetFlag(SITTING))
		{
			m_HeadColl->Transform()->SetRelativePos(Vec3(-15.f, 110.f, 0.f));
		}
		else
		{
			m_HeadColl->Transform()->SetRelativePos(Vec3(-15.f, 170.f, 0.f));
		}
		m_HeadColl->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 20.f));
		m_bLean = true;
	}


	// 기울이기 해제
	if ((KEY_RELEASED(KEY::Q) || KEY_RELEASED(KEY::E)) && !pCamScript->GetFlag(LAYING))
	{
		// 현재 무기 슬롯이 총이라면
		if (m_InventoryScript->GetCurSlotIdx() <= SECONDARY_FIRST)
		{
			WeaponController* pGunScript = m_InventoryScript->GetCurWeaponController();
			pGunScript->ClearKey();
		}

		// Head Coll 이동
		m_HeadColl->Transform()->SetRelativePos(Vec3(0.f, 170.f, 0.f));
		m_HeadColl->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
		m_bLean = false;
	}


	// 앉아있는 상태
	if (pCamScript->GetFlag(SITTING) && pCamScript->GetFlag(CHANGE_STANCE))
	{
		BoxCollider()->SetScale(Vec3(555.f, 900.f, 385.f));
		BoxCollider()->SetOffset(Vec3(35.f, 550.f, 0.f));
		m_HeadColl->Transform()->SetRelativePos(Vec3(0.f, 110.f, 0.f));
		m_HeadColl->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	}


	// 누워있는 상태
	if (pCamScript->GetFlag(LAYING) && pCamScript->GetFlag(CHANGE_STANCE))
	{
		BoxCollider()->SetScale(Vec3(500.f, 480.f, 1475.f));
		BoxCollider()->SetOffset(Vec3(35.f, 25.f, 250.f));
		m_HeadColl->Transform()->SetRelativePos(Vec3(0.f, 5.f, -60.f));
		m_HeadColl->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
		m_bLean = false;
	}

	// 평상시로
	if (!pCamScript->GetFlag(SITTING) && !pCamScript->GetFlag(LAYING) && pCamScript->GetFlag(CHANGE_STANCE))
	{
		BoxCollider()->SetScale(Vec3(550.f, 1600.f, 385.f));
		BoxCollider()->SetOffset(Vec3(35.f, 760.f, 0.f));
		m_HeadColl->Transform()->SetRelativePos(Vec3(0.f, 170.f, 0.f));
		m_HeadColl->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	}

}
void PlayerCharacter::PlayerControlWeapon()
{
	bool bSearch = CGameMgr::GetInst()->GetCamScript()->GetFlag(SEARCH);

	if (!bSearch && !m_InventoryOpened)
	{
		int curSlot = m_InventoryScript->GetCurSlotIdx();

		// 무기가 바꼈다면 IDLE STATE로
		if (m_InventoryScript->IsChange())
		{
			StateMachine()->SetCanExit(true);
			StateMachine()->SetChange(L"Player_Idle");
		}

		// 무기 슬롯이 아니라면 리턴
		if (!(PRIMARY_FIRST <= curSlot && curSlot <= THROWABLE_SECOND))
		{
			return;
		}


		WeaponController* pWeaponController = m_InventoryScript->GetCurWeaponController();
		assert(pWeaponController != nullptr);

		// ======
		// 좌클릭
		// ======

		if (KEY_TAP(KEY::LBTN))
		{
			// 현재 총기라면 Fire State 로 변경
			if (PRIMARY_FIRST <= curSlot && curSlot <= SECONDARY_FIRST)
			{
				StateMachine()->SetChange(L"Player_Gun_Fire");
			}
			// 현재 투척무기라면 Grenade Prepare State 로 변경
			if (THROWABLE_FIRST <= curSlot && curSlot <= THROWABLE_SECOND)
			{
				StateMachine()->SetChange(L"Player_Grenade_Prepare");
			}
			pWeaponController->SetCurKey(KEY::LBTN);
			pWeaponController->SetCurKeyState(KEY_STATE::TAP);
		}
		else if (KEY_RELEASED(KEY::LBTN))
		{
			pWeaponController->SetCurKey(KEY::LBTN);
			pWeaponController->SetCurKeyState(KEY_STATE::RELEASED);
		}

		// ======
		// 우클릭
		// ======

		if (KEY_TAP(KEY::RBTN))
		{
			// 현재 투척무기라면 Grenade Prepare State 로 변경
			if (THROWABLE_FIRST <= curSlot && curSlot <= THROWABLE_SECOND)
			{
				StateMachine()->SetChange(L"Player_Grenade_Prepare");
			}
			pWeaponController->SetCurKey(KEY::RBTN);
			pWeaponController->SetCurKeyState(KEY_STATE::TAP);
		}
		else if (KEY_PRESSED(KEY::RBTN))
		{
			pWeaponController->SetCurKey(KEY::RBTN);
			pWeaponController->SetCurKeyState(KEY_STATE::PRESSED);
		}
		else if (KEY_RELEASED(KEY::RBTN))
		{
			pWeaponController->SetCurKey(KEY::RBTN);
			pWeaponController->SetCurKeyState(KEY_STATE::RELEASED);
		}

		// ===
		// R키
		// ===
		if (KEY_TAP(KEY::R))
		{
			if (CanReload())
			{
				pWeaponController->SetCurKey(KEY::R);
				pWeaponController->SetCurKeyState(KEY_STATE::TAP);
				StateMachine()->SetChange(L"Player_Gun_Reload");
			}
		}

		// ===
		// B키
		// ===
		if (KEY_TAP(KEY::B))
		{
			if (GetStateEnum() == PLAYER_STATE::Player_Idle)
			{
				pWeaponController->SetCurKey(KEY::B);
				pWeaponController->SetCurKeyState(KEY_STATE::TAP);
			}
		}
	}
}


void PlayerCharacter::PlayerControlUI()
{
	CameraController* pCamScript = CGameMgr::GetInst()->GetCamScript();

	// 인벤토리 UI Toggle
	if (KEY_TAP(KEY::TAB) && !m_PauseUIOpened)
	{
		m_InventoryOpened = !m_InventoryOpened;

		// 인벤토리가 열릴때, 상태를 초기화해준다.
		if (m_InventoryOpened)
		{
			if (pCamScript->GetFlag(WAS_TPS))
			{
				pCamScript->SetFlag(ADS, false);
				pCamScript->SetFlag(WAS_TPS, false);
				pCamScript->ChangePS(true);
			}
			else
			{
				pCamScript->SetFlag(ADS, false);
			}
			pCamScript->SetFlag(SHOULDER, false);
		}

		SetMouseActive(m_InventoryOpened);
		SetObjectActive(CGameMgr::GetInst()->GetInventoryCanvasUI(), m_InventoryOpened);
	}

	// Pause UI Toggle
	if (KEY_TAP(KEY::ESC) && !m_OptionUIOpened)
	{
		m_PauseUIOpened = !m_PauseUIOpened;

		// 메뉴가 열릴때, 상태를 초기화해준다.
		if (m_PauseUIOpened)
		{
			if (pCamScript->GetFlag(WAS_TPS))
			{
				pCamScript->SetFlag(ADS, false);
				pCamScript->SetFlag(WAS_TPS, false);
				pCamScript->ChangePS(true);
			}
			else
			{
				pCamScript->SetFlag(ADS, false);
			}
			pCamScript->SetFlag(SHOULDER, false);
		}

		if (m_InventoryOpened)
		{
			SetMouseActive(true);
		}
		else
		{
			SetMouseActive(m_PauseUIOpened);
		}
		SetObjectActive(CGameMgr::GetInst()->GetPauseCanvasUI(), m_PauseUIOpened);			
	}

	// 방위 UI : y축 회전값 전달
	CGameMgr::GetInst()->UpdateCardinalUI(Transform()->GetRelativeRotation().y);


	// HP UI : 현재 체력과 부스트에 대한 정보
	if (IS_HEAL(static_cast<UINT>(m_HealType)))
	{
		CGameMgr::GetInst()->SetHPUI(m_CurHP, m_SemiMaxHP, m_MaxHP, m_CurBoost, m_MaxBoost, m_HealAmount);
	}
	else
	{
		CGameMgr::GetInst()->SetHPUI(m_CurHP, m_SemiMaxHP, m_MaxHP, m_CurBoost, m_MaxBoost);
	}

	// HP상태에 따른 화면 효과
	if (m_CurHP / m_MaxHP <= 0.4f)
	{
		CGameMgr::GetInst()->GetCamEffect()->HPLow();
	}
	else
	{
		CGameMgr::GetInst()->GetCamEffect()->HPFine();
	}
}

void PlayerCharacter::PlayerHeal()
{
	// Boost가 남아있다면
	if (m_CurBoost > 0.f)
	{
		// 특정 시간마다 boost를 줄이고 체력을 채워야 함
		m_BoostRemainTime -= DT;
		m_CurBoost -= m_BoostUnit * DT;	// 부드럽게 빼줌

		// Boost 수행
		if (m_BoostRemainTime <= 0.f)
		{
			// 부스트 비율에 따라 적용 효과가 다름
			float boostRatio = m_CurBoost / m_MaxBoost;

			float boostHP = 0.f;

			if (boostRatio >= 0.9f)
			{
				boostHP = 4.f;
				m_BoostSpeed = 1.0625f;
			}
			else if (boostRatio >= 0.6f)
			{
				boostHP = 3.f;
				m_BoostSpeed = 1.025f;
			}
			else if (boostRatio >= 0.2f)
			{
				boostHP = 2.f;
				m_BoostSpeed = 1.f;
			}
			else
			{
				boostHP = 1.f;
				m_BoostSpeed = 1.f;
			}

			m_CurHP = min(m_CurHP + boostHP, m_MaxHP);

			// 초기 값 세팅
			if (m_CurBoost > 0.f)
			{
				m_BoostRemainTime = m_BoostTotalTime;
			}
			else
			{
				m_BoostRemainTime = 0.f;
			}
		}
	}
}

void PlayerCharacter::DamageCalcul(CGameObject* _AtkObj, CGameObject* _Weapon, float _Damage)
{
	m_CurHP -= _Damage;
	// Hit Sound 출력
	if (!m_bHitSoundPlayed)
	{
		m_HitSoundIdx = FSoundManager::GetInst()->Play3DSound(m_HitSound, Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, false, false, m_HitSoundIdx);
		m_bHitSoundPlayed = true;
	}

	// 사망
	if (m_CurHP <= 0)
	{
		m_CurHP = 0;
		CGameMgr::GetInst()->GetKillInfoUI()->SetKillInfo(_AtkObj->GetName(), GetOwner()->GetName(), _Weapon->GetName());
		CGameMgr::GetInst()->GetKillInfoUI()->OnEvent();

		// 상태
		StateMachine()->SetChange(L"Player_Idle");
		//ChangeState(L"Player_Dead");
	}

	// 피격 화면 효과
	CGameMgr::GetInst()->GetCamEffect()->HitEffect();
}

void PlayerCharacter::Heal()
{
	UINT healType = static_cast<UINT>(m_HealType);

	// 아이템 사용
	m_InventoryScript->UseItem(m_HealType);

	// 회복 수행
	if (IS_HEAL(healType))
	{
		// 의료용 키트라 풀피 채워야 되면
		if (m_HealAmount == m_MaxHP)
		{
			m_CurHP = m_MaxHP;
		}

		// 나머지는 SemiMaxHP 기준으로 회복
		else
		{
			m_CurHP = min(m_CurHP + m_HealAmount, m_SemiMaxHP);
		}
	}
	else // if (IS_BOOST(type))
	{
		m_CurBoost = min(m_CurBoost + m_HealAmount, m_MaxBoost);
	}

	// 값 초기화
	m_HealType = ITEM_TYPE::END;
	m_HealAmount = 0.f;

	SetObjectActive(CGameMgr::GetInst()->GetItemUseUI(), false);
}

void PlayerCharacter::SetMouseActive(bool _b)
{
	m_bMouseActive = _b;

	// 마우스 가둠
	if (!m_bMouseActive)
	{
		CKeyMgr::GetInst()->SetCursorVisible(false);
		CKeyMgr::GetInst()->SetMousePosAsCenter();
	}
	else
	{
		CKeyMgr::GetInst()->SetCursorVisible(true);
	}
}

void PlayerCharacter::TriggerHeal(ITEM_TYPE PHealType)
{
	// 고려사항
	// 1. item 사용 딜레이
	// 2. 초당 힐량
	// 3. 최대 힐량	(최대 몇까지 힐을 한다)
	// 4. 총 힐량    (최대 제한은 없고 다 사용했을 때 힐량)

	bool CantHeal = false;

	switch (PHealType)
	{
	case ITEM_TYPE::FIRST_AID_KIT:
		if (m_CurHP >= m_SemiMaxHP)
		{
			CantHeal = true;
			break;
		}
		m_HealTotalTime = m_HealRemainTime = 6.f;
		m_HealAmount = 75.f;
		break;
	case ITEM_TYPE::MED_KIT:
		if (m_CurHP >= m_MaxHP)
		{
			CantHeal = true;
			break;
		}
		m_HealTotalTime = m_HealRemainTime = 8.f;
		m_HealAmount = 100.f;
		break;
	case ITEM_TYPE::BANDAGE:
		if (m_CurHP >= m_SemiMaxHP)
		{
			CantHeal = true;
			break;
		}
		m_HealTotalTime = m_HealRemainTime = 4.f;
		m_HealAmount = 10.f;
		break;
	case ITEM_TYPE::ADRENALINE_SYRINGE:
		m_HealTotalTime = m_HealRemainTime = 6.f;
		m_HealAmount = 100.f;
		break;
	case ITEM_TYPE::PAIN_KILLER:
		m_HealTotalTime = m_HealRemainTime = 6.f;
		m_HealAmount = 60.f;
		break;
	case ITEM_TYPE::ENERGY_DRINK:
		m_HealTotalTime = m_HealRemainTime = 4.f;
		m_HealAmount = 40.f;
		break;
	default:
		break;
	}

	m_HealType = PHealType;
	StateMachine()->SetChange(L"Player_Heal");

	if (CantHeal)
	{
		// TODO(Ssio) : 사용 할 수 없다는 경고 문구 UI
		return;
	}

	// ui 활성화
	SetObjectActive(CGameMgr::GetInst()->GetItemUseUI(), true);
}

void PlayerCharacter::LoadPlayerSounds()
{
	m_HitSound = FAssetManager::GetInst()->Load<FSound>(L"Sound\\player_hit.mp3");
	m_FootstepSound = FAssetManager::GetInst()->Load<FSound>(L"Sound\\player_footstep.mp3");
	m_RunFootstepSound = FAssetManager::GetInst()->Load<FSound>(L"Sound\\player_footstep_faster.mp3");
}


PLAYER_STATE PlayerCharacter::GetStateEnum() const
{
	return WstringToEnum<PLAYER_STATE>(StateMachine()->GetCurState()->GetName());
}

void PlayerCharacter::SaveComponent(FILE* PFile)
{
	//fwrite(&m_PlayerSpeed, sizeof(float), 1, _File);
	SaveAssetRef(m_TargetTex, PFile);
}

void PlayerCharacter::LoadComponent(FILE* PFile)
{
	//fread(&m_PlayerSpeed, sizeof(float), 1, _File);
	LoadAssetRef(m_TargetTex, PFile);
}

void PlayerCharacter::LoadComponentReference()
{
}

void PlayerCharacter::ProgressHealState()
{
	// 회복 아이템 사용중이면
	m_HealRemainTime -= DT;

	// 완료
	if (m_HealRemainTime <= 0.f)
	{
		Heal();

		// 상태
		StateMachine()->SetChange(L"Player_Idle");
	}

	// 진행 중
	else
	{
		// ItemUseUI : 아이템 사용딜레이 ui
		CGameMgr::GetInst()->SetItemUseUITime(m_HealRemainTime, m_HealTotalTime);
	}
}


void PlayerCharacter::ProgressReloadState()
{
	m_InteractionScript->SetInteractable(true);

	// 재장전 진행
	// REFACTOR :현재 무기의 Script를 static_cast를 통해 받아오는 구조가 맘에 들진 않음
	// 만약 개선 방법이 보인다면 변경하면 좋을 것 같음.

	GunController* curWeaponScript = static_cast<GunController*>(m_InventoryScript->GetCurWeaponController());
	if (curWeaponScript == nullptr)
	{
		return;
	}
	float fReloadTime = curWeaponScript->GetReloadingTime();

	m_StateAccTime += DT;

	if (fReloadTime < m_StateAccTime)
	{
		// State 전환
		StateMachine()->SetCanExit(true);
		StateMachine()->SetChange(L"Player_Idle");
	}

	// 인벤토리가 열린 상태에서는 조작 불가능
	if (!m_InventoryOpened)
	{
		// F 키를 누르면 즉시 재장전을 취소한다.
		if (KEY_TAP(KEY::F))
		{
			WeaponController* pGunScript = m_InventoryScript->GetCurWeaponController();
			pGunScript->SetCurKey(KEY::F);
			pGunScript->SetCurKeyState(KEY_STATE::TAP);
			StateMachine()->SetCanExit(true);
			StateMachine()->SetChange(L"Player_Idle");
		}
	}
}

void PlayerCharacter::ProgressFireState()
{
	GunController* curWeaponScript = static_cast<GunController*>(m_InventoryScript->GetCurWeaponController());\
	if (curWeaponScript == nullptr)
	{
		return;
	}
	bool bPullTigger = curWeaponScript->IsPullTrigger();


	// PRESSED KEY 입력은 Gun에게만 넘긴다
	if (KEY_PRESSED(KEY::LBTN))
	{

		curWeaponScript->SetCurKey(KEY::LBTN);
		curWeaponScript->SetCurKeyState(KEY_STATE::PRESSED);
	}

	m_StateAccTime += DT;
	if (!bPullTigger && 0.2f < m_StateAccTime)
	{
		// State 전환
		StateMachine()->SetChange(L"Player_Idle");
	}
}

void PlayerCharacter::ProgressThrowPrepareState(bool _InputThrow, bool _LBTN)
{
	WeaponController* pWeaponController = m_InventoryScript->GetCurWeaponController();
	if (pWeaponController == nullptr)
	{
		// Tigger 발동으로 준비동작에서 수류탄이 터짐
		// DamgeCal에서 SetChange와 순서고려
		StateMachine()->SetChange(L"Player_Idle");
		return;
	}

	// Trigger 작동
	if (KEY_TAP(KEY::R))
	{
		pWeaponController->SetCurKey(KEY::R);
		pWeaponController->SetCurKeyState(KEY_STATE::TAP);
	}

	if(_InputThrow)
	{
		if (_LBTN)
		{
			StateMachine()->SetChange(L"Player_Grenade_Throw_High");
		}
		else
		{
			StateMachine()->SetChange(L"Player_Grenade_Throw_Low");
		}
	}
}

void PlayerCharacter::ProgressThrowState()
{
	m_StateAccTime += DT;
	if (1.7f < m_StateAccTime)
	{
		StateMachine()->SetChange(L"Player_Idle");
	}
}

void PlayerCharacter::ExitThrowPrepareState()
{
	if (m_InventoryScript->GetCurWeapon() != nullptr)
	{
		// 인벤토리에서 투척무기 하나 제거
		ITEM_TYPE type = static_cast<ItemScript*>(GetScriptWithType(m_InventoryScript->GetCurWeapon(), SCRIPT_TYPE::ITEMSCRIPT))->GetItemType();
		m_InventoryScript->UseItem(type, 1);

		// 아이템이 남지 않았다면
		/*if (!)
		{
			m_bCanThrow = false;
		}*/
	}
}

void PlayerCharacter::ExitReloadState()
{
	SetObjectActive(CGameMgr::GetInst()->GetReloadUI(), false);
	m_InteractionScript->SetInteractable(false);
}


bool PlayerCharacter::CanReload()
{
	int curSlot = m_InventoryScript->GetCurSlotIdx();
	if (PRIMARY_SECOND < curSlot)
	{
		return false;
	}
	GunController* pWeaponScript = static_cast<GunController*>(m_InventoryScript->GetCurWeaponController());
	int iLeftRounds = m_InventoryScript->GetItemCount(pWeaponScript->GetRoundType());
	int iCurRounds = pWeaponScript->GetCurRound();
	if (iLeftRounds == 0)
	{
		return false;
	}
	if (iCurRounds == pWeaponScript->GetMaxRound())
	{
		return false;
	}
	if (GetStateEnum() == PLAYER_STATE::Player_Gun_Reload)
	{
		return false;
	}

	return true;
}
