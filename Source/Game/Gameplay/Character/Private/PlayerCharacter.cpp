#include "pch.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Rendering/Device/CDevice.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"

#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/UI/Public/KillinfoUIScript.h"
#include "Game/Gameplay/Inventory/Public/Item.h"


PlayerCharacter::PlayerCharacter()
	: CScript(SCRIPT_TYPE::PLAYERSCRIPT)
	, m_MainCamera(nullptr)
	, m_MouseSensitivity(10.f)
	, m_Force(0.f)
	, m_Velocity(0.f)
	, m_GravityVelocity(0.f)
	, m_Mass(3.f)
	, m_Friction(100.f)
	, m_MaxSpeed(50.f)
	, m_GravityAccel(10.f)
	, m_GravityMaxSpeed(30.f)
	, m_JumpPower(5.f)
	, m_IsGround(true)
	, m_bLean(false)
	, m_bShoot(false)
	, m_bCanThrow(false)
	, m_bThrowBoom(false)
	, m_CollObject(nullptr)
	, m_HeadColl(nullptr)
	, m_CamScript(nullptr)
	, m_InventoryScript(nullptr)
	, m_KillinfoScript(nullptr)
	, m_MaxHP(100.f)
	, m_SemiMaxHP(75.f)
	, m_CurHP(100.f)
	, m_MaxBoost(100.f)
	, m_CurBoost(0.f)
	, m_HealType(ITEM_TYPE::END)
	, m_RemainTime(0.f)
	, m_TotalTime(0.f)
	, m_HealAmount(0.f)
	, m_BoostRemainTime(0.f)
	, m_BoostTotalTime(8.f)
	, m_BoostUnit(0.3f)
	, m_BoostSpeed(1.f)
	, m_InventoryCanvasUI(nullptr)
	, m_InventoryOpened(false)
	, m_CardinalImageUI(nullptr)
	, m_HPUI(nullptr)
	, m_ItemUseUI(nullptr)
	, m_KillCounts(0)
{
	AddScriptParam(tScriptParam{SCRIPT_PARAM::FLOAT, "Player Mass", &m_Mass });				// 질량
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "Friction", &m_Friction });	// 마찰계수
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "MaxSpeed", &m_MaxSpeed });	// 최고속도
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "GravityAccel", &m_GravityAccel });	// 중력 가속도
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "GravityMaxSpeed", &m_GravityMaxSpeed });	// 중력 최대속도

	//AddScriptParam(tScriptParam{SCRIPT_PARAM::TEXTURE, "Test Texture", &m_TargetTex});
	//AddScriptParam(tScriptParam{SCRIPT_PARAM::PREFAB, "Missile", &m_Prefab});


	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "CurHP", &m_CurHP });
}

PlayerCharacter::~PlayerCharacter()
{
}


void PlayerCharacter::Begin()
{
	// TODO : ObjectReference로 변경하기
	m_MainCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");

	// Collider
	m_HeadColl = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player Head");

	// UI
	m_InventoryCanvasUI = CLevelMgr::GetInst()->FindObjectByName(L"Inventory_CanvasUI");
	m_CardinalImageUI = CLevelMgr::GetInst()->FindObjectByName(L"Cardinal_ImageUI");

	m_HPUI = CLevelMgr::GetInst()->FindObjectByName(L"HP_UI");

	Vec2 uiSize = m_HPUI->UI()->GetRectSize();
	m_HPUI->UIRender()->GetMaterial(0)->SetScalarParam(VEC2_0, Vec2(uiSize.x / uiSize.y, m_SemiMaxHP / m_MaxHP));	// 고정 값
	m_HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, m_CurHP / m_MaxHP);
	m_HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, m_CurBoost / m_MaxBoost);

	m_ItemUseUI = CLevelMgr::GetInst()->FindObjectByName(L"ItemUse_UI");

	// Script
	m_CamScript = static_cast<CameraController*>(GetScriptWithType(m_MainCamera, SCRIPT_TYPE::CAMERASCRIPT));
	m_InventoryScript = static_cast<InventoryController*>(GetScriptWithType(GetOwner(), SCRIPT_TYPE::INVENTORYSCRIPT));
	CGameObject* killinfoUI = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Killinfo_UI");
	m_KillinfoScript = static_cast<KillinfoUIScript*>(GetScriptWithType(killinfoUI, SCRIPT_TYPE::KILLINFOUI));
	// 마우스 끄기
	CKeyMgr::GetInst()->SetCursorVisible(false);
}

void PlayerCharacter::Tick()
{
	// 여기서 조건을 모두 판정함.

	// =================
	// 항상 작동하는 로직
	// =================

	// 마우스가 켜진 상태에서 키입력 방지,
	if (!m_bMouseActive && !m_CamScript->GetFlag(FREE_PS))
	{
		// 이동 로직
		PlayerMove();

		// 행동 로직
		PlayerStance();
	}


	// UI 관리
	PlayerControlUI();

	// Heal 상태
	PlayerHeal();


	// ==========
	// 조건부 로직
	// ==========

	if (KEY_TAP(KEY::F1))
	{
		m_bMouseActive = !m_bMouseActive;

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

	if (!m_CamScript->GetFlag(FREE_PS))
	{
		if (KEY_TAP(KEY::ESC) || KEY_TAP(KEY::TAB))
		{
			m_bMouseActive = !m_bMouseActive;

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

		PlayerAttack();
		PlayerReload();
	}
}


void PlayerCharacter::PlayerView()
{
	bool bRecover = m_CamScript->GetFlag(SEARCH_RECOVER);
	bool bSearch = m_CamScript->GetFlag(SEARCH);
	bool bShoulder = m_CamScript->GetFlag(SHOULDER);
	bool bADS = m_CamScript->GetFlag(ADS);
	bool bTPS = m_CamScript->GetFlag(TPS);

	// 마우스 위치를 구해온다
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();
	Vec3 vPlayerRot = Transform()->GetRelativeRotation();
	Vec3 vCameraRot = m_MainCamera->Transform()->GetRelativeRotation();

	// 화면의 중앙을 구한다.
	static Vec2 vResoulution = CDevice::GetInst()->GetRenderResolution();
	static int centerX = static_cast<int>(vResoulution.x / 2);
	static int centerY = static_cast<int>(vResoulution.y / 2);

	// 현재 마우스위치와 화면 정중앙의 변화값을 구한다.
	int deltaX = static_cast<int>(vMousePos.x - centerX);
	int deltaY = static_cast<int>(vMousePos.y - centerY);

	// 미세한 움직임으로 인한 회전을 방지하기 위해 Deadzone을 추가한다.
	int deadzone = 5;
	if (abs(deltaX) < deadzone)
	{
		deltaX = 0;
	}
	else
	{
		deltaX = (deltaX > 0) ? (deltaX - deadzone) : (deltaX + deadzone);
	}

	static float OriginRotY = 0.f;

	// 평상시 & TPS 상태일 때
	if (!bADS && !bShoulder && !m_bShoot && bTPS)
	{
		// Search
		if (KEY_TAP(KEY::LCTRL))
		{
			OriginRotY = vCameraRot.y;
		}

		if (KEY_PRESSED(KEY::LCTRL))
		{
			// 줌 하는 동안 둘러보기 키가 눌린경우를 방지한다.
			if (bSearch)
			{
				vCameraRot.y += deltaX * m_MouseSensitivity * DT;
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
		else
		{
			if (!bRecover)
			{
				vPlayerRot.y += deltaX * m_MouseSensitivity * DT;
			}
		}
	}

	// 줌 혹은 사격중
	else
	{
		// 해당 변화값으로 Player 회전 적용
		if (!bRecover)
		{
			vPlayerRot.y += deltaX * m_MouseSensitivity * DT;
		}
	}

	// 위아래 회전
	if (!bRecover)
	{
		vCameraRot.x += deltaY * m_MouseSensitivity * DT;
		if (vCameraRot.x < -90.f)
		{
			vCameraRot.x = -90.f;
		}
		else if (80.f < vCameraRot.x)
		{
			vCameraRot.x = 80.f;
		}

		m_MainCamera->Transform()->SetRelativeRotation(vCameraRot);
	}

	Transform()->SetRelativeRotation(vPlayerRot);


	// 마우스 위치를 중앙으로 다시 초기화한다.
	CKeyMgr::GetInst()->SetMousePosAsCenter();
	Vec3 RayDir = Vec3(0.f, 0.f, 0.f);
	float angle = vCameraRot.x * (XM_PI / 180.f);
	RayDir.y = -sin(angle);
	RayDir.z = fabs(RayDir.y) - 1;
	ColliderRay()->SetRayDir(RayDir);
}

void PlayerCharacter::PlayerStance()
{
	// 기울이기
	if (KEY_PRESSED(KEY::Q) && !m_CamScript->GetFlag(LAYING))
	{
		// 현재 무기 슬롯이 총이라면
		if (m_InventoryScript->GetCurSlotIdx() <= SECONDARY_FIRST)
		{
			WeaponController* pGunScript = m_InventoryScript->GetCurWeaponController();
			pGunScript->SetCurKey(KEY::Q);
			pGunScript->SetCurKeyState(KEY_STATE::PRESSED);
		}

		// Head Coll 이동
		if (m_CamScript->GetFlag(SITTING))
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

	if (KEY_PRESSED(KEY::E) && !m_CamScript->GetFlag(LAYING))
	{
		// 현재 무기 슬롯이 총이라면
		if (m_InventoryScript->GetCurSlotIdx() <= SECONDARY_FIRST)
		{
			WeaponController* pGunScript = m_InventoryScript->GetCurWeaponController();
			pGunScript->SetCurKey(KEY::E);
			pGunScript->SetCurKeyState(KEY_STATE::PRESSED);
		}


		// Head Coll 이동
		if (m_CamScript->GetFlag(SITTING))
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
	if ((KEY_RELEASED(KEY::Q) || KEY_RELEASED(KEY::E)) && !m_CamScript->GetFlag(LAYING))
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
	if (m_CamScript->GetFlag(SITTING) && m_CamScript->GetFlag(CHANGE_STANCE))
	{
		Collider3D()->SetScale(Vec3(555.f, 900.f, 385.f));
		Collider3D()->SetOffset(Vec3(35.f, 550.f, 0.f));
		m_HeadColl->Transform()->SetRelativePos(Vec3(0.f, 110.f, 0.f));
		m_HeadColl->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	}


	// 누워있는 상태
	if (m_CamScript->GetFlag(LAYING) && m_CamScript->GetFlag(CHANGE_STANCE))
	{
		Collider3D()->SetScale(Vec3(500.f, 480.f, 1475.f));
		Collider3D()->SetOffset(Vec3(35.f, 25.f, 250.f));
		m_HeadColl->Transform()->SetRelativePos(Vec3(0.f, 5.f, -60.f));
		m_HeadColl->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
		m_bLean = false;
	}

	// 평상시로
	if (!m_CamScript->GetFlag(SITTING) && !m_CamScript->GetFlag(LAYING) && m_CamScript->GetFlag(CHANGE_STANCE))
	{
		Collider3D()->SetScale(Vec3(550.f, 1600.f, 385.f));
		Collider3D()->SetOffset(Vec3(35.f, 760.f, 0.f));
		m_HeadColl->Transform()->SetRelativePos(Vec3(0.f, 170.f, 0.f));
		m_HeadColl->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	}

}

void PlayerCharacter::PlayerReload()
{
	if (KEY_TAP(KEY::R))
	{
		// 현재 무기 슬롯이 총이라면
		if (m_InventoryScript->GetCurSlotIdx() <= SECONDARY_FIRST)
		{
			WeaponController* pGunScript = m_InventoryScript->GetCurWeaponController();
			pGunScript->SetCurKey(KEY::R);
			pGunScript->SetCurKeyState(KEY_STATE::TAP);
		}
	}
}

void PlayerCharacter::PlayerAttack()
{
	bool bSearch = m_CamScript->GetFlag(SEARCH);

	if (!bSearch && !m_InventoryOpened)
	{
		WeaponController* pWeaponController = m_InventoryScript->GetCurWeaponController();

		if (KEY_TAP(KEY::RBTN))
		{
			// 현재 무기 슬롯이 총이라면
			if (m_InventoryScript->GetCurSlotIdx() <= THROWABLE_SECOND)
			{
				pWeaponController->SetCurKey(KEY::RBTN);
				pWeaponController->SetCurKeyState(KEY_STATE::TAP);
			}
		}

		if (KEY_TAP(KEY::LBTN))
		{
			// 현재 무기 슬롯이 총이라면
			if (m_InventoryScript->GetCurSlotIdx() <= THROWABLE_SECOND)
			{
				pWeaponController->SetCurKey(KEY::LBTN);
				pWeaponController->SetCurKeyState(KEY_STATE::TAP);
			}
		}

		if (KEY_PRESSED(KEY::LBTN))
		{
			// 총을 사용한다.
			if (m_bShoot)
			{
				pWeaponController->SetCurKey(KEY::LBTN);
				pWeaponController->SetCurKeyState(KEY_STATE::PRESSED);
			}

			if (m_bCanThrow)
			{
				if (KEY_TAP(KEY::R))
				{
					pWeaponController->SetCurKey(KEY::R);
					pWeaponController->SetCurKeyState(KEY_STATE::TAP);
				}
			}
		}

		if (KEY_RELEASED(KEY::LBTN))
		{
			// 총기
			if (m_bShoot)
			{
				m_bShoot = false;
				pWeaponController->SetCurKey(KEY::LBTN);
				pWeaponController->SetCurKeyState(KEY_STATE::RELEASED);
			}

			// 투척무기
			if (m_bCanThrow)
			{
				if (m_InventoryScript->GetCurWeapon() != nullptr)
				{
					pWeaponController->SetCurKey(KEY::LBTN);
					pWeaponController->SetCurKeyState(KEY_STATE::RELEASED);

					// 인벤토리에서 투척무기 하나 제거
					ITEM_TYPE type = static_cast<ItemScript*>(GetScriptWithType(m_InventoryScript->GetCurWeapon(), SCRIPT_TYPE::ITEMSCRIPT))->GetItemType();

					// 아이템이 남았다면
					if (m_InventoryScript->UseItem(type, 1))
					{
						// TODO: object pooling으로 개선
						Ptr<CPrefab> pPrefab = ItemMgr::GetInst()->GetItemInfo(type).Prefab;
						CGameObject* newItem = pPrefab->Instantiate();
						CreateObject(newItem, 0, false);

						// REUSE : 기존 슬롯에 prefab을 장착
						m_InventoryScript->EquipSlot(newItem, type, m_InventoryScript->GetCurSlotIdx(), false);

						// 손에 부착
						m_InventoryScript->ActivateSlot(m_InventoryScript->GetCurSlotIdx());
					}

					// 아이템이 남지 않았다면
					else
					{
						m_InventoryScript->ClearSlot(m_InventoryScript->GetCurSlotIdx());
						m_bCanThrow = false;
					}
				}
			}
		}

		if (KEY_TAP(KEY::RBTN))
		{
			// 투척무기이면
			if (THROWABLE_FIRST <= m_InventoryScript->GetCurSlotIdx() && m_InventoryScript->GetCurSlotIdx() <= THROWABLE_SECOND)
			{
				WeaponController* pWeaponScript = m_InventoryScript->GetCurWeaponController();
				pWeaponScript->SetCurKey(KEY::RBTN);
				pWeaponScript->SetCurKeyState(KEY_STATE::TAP);
			}
		}

		if (KEY_PRESSED(KEY::RBTN))
		{
			if (m_bCanThrow)
			{
				if (KEY_TAP(KEY::R))
				{
					WeaponController* pWeaponScript = m_InventoryScript->GetCurWeaponController();
					pWeaponScript->SetCurKey(KEY::R);
					pWeaponScript->SetCurKeyState(KEY_STATE::TAP);
				}
			}
		}

		if (KEY_RELEASED(KEY::RBTN))
		{
			// 투척무기
			if (m_bCanThrow)
			{
				if (m_InventoryScript->GetCurWeapon() != nullptr)
				{
					WeaponController* pWeaponScript = m_InventoryScript->GetCurWeaponController();
					pWeaponScript->SetCurKey(KEY::RBTN);
					pWeaponScript->SetCurKeyState(KEY_STATE::RELEASED);

					// 인벤토리에서 투척무기 하나 제거
					ITEM_TYPE type = static_cast<ItemScript*>(GetScriptWithType(m_InventoryScript->GetCurWeapon(), SCRIPT_TYPE::ITEMSCRIPT))->GetItemType();

					// 아이템이 남았다면
					if (m_InventoryScript->UseItem(type, 1))
					{
						// TODO: object pooling으로 개선
						Ptr<CPrefab> pPrefab = ItemMgr::GetInst()->GetItemInfo(type).Prefab;
						CGameObject* newItem = pPrefab->Instantiate();
						CreateObject(newItem, 0, false);

						// REUSE : 기존 슬롯에 prefab을 장착
						m_InventoryScript->EquipSlot(newItem, type, m_InventoryScript->GetCurSlotIdx());

						// 손에 부착
						m_InventoryScript->ActivateSlot(m_InventoryScript->GetCurSlotIdx());
					}

					// 아이템이 남지 않았다면
					else
					{
						m_InventoryScript->ClearSlot(m_InventoryScript->GetCurSlotIdx());
						m_bCanThrow = false;
					}
				}
			}
		}

		if (KEY_TAP(KEY::B))
		{
			// 현재 무기 슬롯이 총이라면
			if (m_InventoryScript->GetCurSlotIdx() <= THROWABLE_SECOND)
			{
				pWeaponController->SetCurKey(KEY::B);
				pWeaponController->SetCurKeyState(KEY_STATE::TAP);
			}
		}
	}
}



void PlayerCharacter::PlayerControlUI()
{
	// 인벤토리 UI Toggle
	if (KEY_TAP(KEY::TAB))
	{
		// 인벤토리가 열릴때, 상태를 초기화해준다.
		if (!m_InventoryOpened)
		{
			if (m_CamScript->GetFlag(WAS_TPS))
			{
				m_CamScript->SetFlag(ADS, false);
				m_CamScript->SetFlag(WAS_TPS, false);
				m_CamScript->ChangePS(true);
			}
			else
			{
				m_CamScript->SetFlag(ADS, false);
			}
			m_CamScript->SetFlag(SHOULDER, false);
		}

		m_InventoryOpened = !m_InventoryOpened;
		SetObjectActive(m_InventoryCanvasUI, m_InventoryOpened);
	}

	// 방위 UI : y축 회전값 전달
	m_CardinalImageUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, Transform()->GetRelativeRotation().y);

	// HP UI : 현재 체력과 부스트에 대한 정보
	m_HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, m_CurHP / m_MaxHP);
	m_HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, m_CurBoost / m_MaxBoost);

	if (IS_HEAL(static_cast<UINT>(m_HealType)))
	{
		if (m_HealAmount == 100.f)
			m_HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_2, 1.f);
		else
			m_HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_2, min(m_CurHP + m_HealAmount, m_SemiMaxHP) / m_MaxHP);
	}
	else
	{
		m_HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_2, 0.f);
	}

}

void PlayerCharacter::PlayerHeal()
{
	UINT type = static_cast<UINT>(m_HealType);

	// 회복 아이템 사용중이면
	if (IS_HEAL(type) || IS_BOOST(type))
	{
		m_RemainTime -= DT;

		// 완료
		if (m_RemainTime <= 0.f)
		{
			m_InventoryScript->UseItem(m_HealType);

			// 회복 수행
			if (IS_HEAL(type))
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
			m_RemainTime = 0.f;
			m_HealAmount = 0.f;

			SetObjectActive(m_ItemUseUI, false);
		}

		// 진행 중
		else
		{
			// ItemUseUI : 아이템 사용딜레이 ui
			m_ItemUseUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, 1.f - m_RemainTime / m_TotalTime);

			// 남은 시간 글씨 출력
			wchar_t text[4]{};	// 3글자 출력
			swprintf_s(text, L"%.1f", m_RemainTime);
			m_ItemUseUI->UI()->GetTextInfoRef()[0].Text = text;
		}
	}

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

void PlayerCharacter::DemageCalcul(CGameObject* PAtkObj, CGameObject* PWeapon, float PDamage)
{
	m_CurHP -= PDamage;

	// 사망
	if (m_CurHP <= 0)
	{
		m_CurHP = 0;
		m_KillinfoScript->SetKillInfo(PAtkObj->GetName(), GetOwner()->GetName(), PWeapon->GetName());
		m_KillinfoScript->OnEvent();
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
		m_TotalTime = m_RemainTime = 6.f;
		m_HealAmount = 75.f;
		break;
	case ITEM_TYPE::MED_KIT:
		if (m_CurHP >= m_MaxHP)
		{
			CantHeal = true;
			break;
		}
		m_TotalTime = m_RemainTime = 8.f;
		m_HealAmount = 100.f;
		break;
	case ITEM_TYPE::BANDAGE:
		if (m_CurHP >= m_SemiMaxHP)
		{
			CantHeal = true;
			break;
		}
		m_TotalTime = m_RemainTime = 4.f;
		m_HealAmount = 10.f;
		break;
	case ITEM_TYPE::ADRENALINE_SYRINGE:
		m_TotalTime = m_RemainTime = 6.f;
		m_HealAmount = 100.f;
		break;
	case ITEM_TYPE::PAIN_KILLER:
		m_TotalTime = m_RemainTime = 6.f;
		m_HealAmount = 60.f;
		break;
	case ITEM_TYPE::ENERGY_DRINK:
		m_TotalTime = m_RemainTime = 4.f;
		m_HealAmount = 40.f;
		break;
	default:
		break;
	}

	if (CantHeal)
	{
		// TODO : 사용 할 수 없다는 경고 문구 UI
		return;
	}

	// ui 활성화
	SetObjectActive(m_ItemUseUI, true);

	m_HealType = PHealType;
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


