#include "pch.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"
#include "Engine/System/Public/Manager/CObjectPoolMgr.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Projectile/Public/MissileProjectile.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"

#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"

GunController::GunController()
	: WeaponController(SCRIPT_TYPE::GUNSCRIPT)
	, m_AkSoundIdx(-1)
	, m_AkdrySoundIdx(-1)
	, m_EmptyReloadSoundIdx(-1)
	, m_ReloadSoundIdx(-1)
	, m_ClipSoundIdx(-1)
	, m_HorizontalRecoilPower(0.f)
	, m_VerticalRecoilPower(0.f)
	, m_FireDelay(0.f)
	, m_InitFirePower(0.f)
	, m_BulletDmg(25.f)
	, m_AccTime_Fire(0.f)
	, m_AccTime_Reload(0.f)
	, m_MaxRounds(0)
	, m_CurRounds(0)
	, m_bFire(false)
	, m_bReload(false)
	, m_bAuto(false)
	, m_PlayerScript(nullptr)
	, m_InventoryScript(nullptr)
	, m_WeaponRoundType()
{
	// 무기 종류에 따라 변수 값 설정
	m_VerticalRecoilPower = 2.2f;
	m_HorizontalRecoilPower = 1.f;

	m_InitFirePower = 20000.f;

	m_FireDelay = 0.1f;
	m_MaxRounds = 30;

	m_ReloadingTime = 2.8f;

	// 무기의 총알 타입 정의
	m_WeaponRoundType = ITEM_TYPE::AMMO_5;
}

GunController::~GunController()
{
}


void GunController::Begin()
{
	WeaponController::Begin();

	// Sound
	m_AkSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\akm_reverb.mp3");
	m_AkdrySound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\gun_dry_reverb.mp3");
	m_ReloadSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\gun_reload.mp3");
	m_EmptyReloadSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\gun_empty_reload.mp3");
	m_ClipSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\gun_clip.mp3");


	// Script
	m_InventoryScript = static_cast<InventoryController*>(GetScriptWithType(CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player"), SCRIPT_TYPE::INVENTORYSCRIPT));

	// UI
	m_ReloadUI = CLevelMgr::GetInst()->FindObjectByName(L"Reload_UI");	
}

void GunController::Tick()
{
	// 소유주가 없다면 return
	if (m_EquippedOwner == nullptr || m_bIsEquipped == false)
	{
		// 진행중이던 상태 모두 초기화
		m_bFire = false;
		m_bReload = false;
		m_AccTime_Reload = 0.f;
		CSoundMgr::GetInst()->Stop3DSound(m_ReloadSoundIdx);
		CSoundMgr::GetInst()->Stop3DSound(m_EmptyReloadSoundIdx);
		return;
	}

	// 연사 시간 간격은 Firing이 아닐 때도 계산해둠.
	// 좌클릭을 누르고 있을 때만 시간이 흐르는 것 방지.
	if (m_FireDelay > m_AccTime_Fire)
		m_AccTime_Fire += DT;

	// 플레이어일 시 처리
	if (!m_bEnemy)
	{
		if (!m_PlayerScript)
			m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_EquippedOwner, SCRIPT_TYPE::PLAYERSCRIPT));

		// 총알을 발사한다.
		if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::TAP)
		{
			if (0 < m_CurRounds && !m_bReload)
			{
				m_PlayerScript->SetShot(true);
				m_bFire = true;
				ClearKey();

				// 상태
				m_PlayerScript->SetActionState(ACTION_STATE::GUN_FIRE);								
			}
			else if (m_CurRounds == 0 && !m_bReload)
			{
				// 빈 탄창 발사 사운드
				m_AkdrySoundIdx = CSoundMgr::GetInst()->Play3DSound(m_AkdrySound, Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, true, true, -1);
				ClearKey();
			}
		}

		if (m_CurKey == KEY::R && m_CurKeyState == KEY_STATE::TAP)
		{		
			if (CanReload())
			{
				m_PlayerScript->SetShot(false);
				m_bReload = true;
				SetObjectActive(m_ReloadUI, true);
				m_PlayerScript->SetReloading(true);

				// 현재 남은 탄창수에 따라 시간 설정
				if (m_CurRounds == 0)
				{
					m_ReloadingTime = 2.8f;
				}
				else
				{
					m_ReloadingTime = 1.5f;
				}					
			}			

			ClearKey();
			// 상태
			m_PlayerScript->SetActionState(ACTION_STATE::GUN_RELOAD);

		}

		if (m_CurKey == KEY::B && m_CurKeyState == KEY_STATE::TAP)
		{
			if (m_bReload)
			{
				ClearKey();
			}
			else
			{
				m_PlayerScript->SetShot(false);
				m_bFire = false;
				m_bAuto = !m_bAuto;
				// 조정간 조정 사운드
				m_ClipSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_ClipSound, Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, true, true, -1);
				ClearKey();
			}
		}

		// 총알을 발사한다.
		if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::PRESSED)
		{
			if (0 < m_CurRounds && m_bAuto)
				m_bFire = true;
		}

		if (m_CurKey == KEY::F && m_CurKeyState == KEY_STATE::TAP && m_bReload)
		{			
			m_bReload = false;
			m_AccTime_Reload = 0.f;
			// 재장전 사운드를 멈춰준다.
			CSoundMgr::GetInst()->Stop3DSound(m_ReloadSoundIdx);
			CSoundMgr::GetInst()->Stop3DSound(m_EmptyReloadSoundIdx);
			ClearKey();			
		}
		AdjustFPSPos();
	}
	// ai일시 처리
	else
	{
		if (m_CurKey == KEY::R && m_CurKeyState == KEY_STATE::TAP && m_CurRounds != m_MaxRounds)
		{
			if (!m_bReload && m_CurRounds != m_MaxRounds)
			{
				m_bReload = true;
			}
		}

		if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::PRESSED)
		{
			if (0 < m_CurRounds)
			{
				m_bFire = true;
				m_bAuto = true;
			}

		}
	}


	// 총알 발사가 끝난다.
	if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::RELEASED)
	{
		StopFiring();
	}

	if (m_bFire && !m_bReload)
	{
		Firing();
	}

	if (m_bReload)
	{
		Reload();
	}
}

bool GunController::CanReload()
{
	return !m_bReload &&
		m_CurRounds != m_MaxRounds &&
		m_InventoryScript->GetItemCount(m_WeaponRoundType) > 0;
}

void GunController::Firing()
{
	// 총알의 시작 위치를 정해준다.
	Vec3 vRayPos = ColliderRay()->GetRayFinalPos();
	Vec3 vFinalDir = GetFireDir();

	// 현재 사격 자세에 따라 정확도를 부여한다.
	float spreadYaw = 0.f;
	float spreadPitch = 0.f;

	// 적이 사용중인 사격
	if (m_bEnemy)
	{
		// 사격 정확도는 일단 임의로 설정
		spreadYaw = RandomFloat(-10.f, 10.f);
		spreadPitch = RandomFloat(-10.f, 10.f);

		// 총알을 모두 소진했다면
		if (m_CurRounds <= 0)
		{
			m_bFire = false;
			return;
		}

	}
	// 플레이어가 사용중인 사격
	else
	{
		// 총알을 모두 소진했다면
		if (m_CurRounds <= 0)
		{
			PlayerCharacter* pPlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_EquippedOwner, SCRIPT_TYPE::PLAYERSCRIPT));

			pPlayerScript->SetShot(false);
			m_bFire = false;
			
			// 빈 탄창 발사 사운드
			m_AkdrySoundIdx = CSoundMgr::GetInst()->Play3DSound(m_AkdrySound, vRayPos, 1.f, 10000.f, 1, 1.f, true, true, -1);
			return;
		}

		// Camera의 줌 여부를 확인한다.
		// 지향사격
		if (!m_CamScript->GetFlag(SHOULDER) && !m_CamScript->GetFlag(ADS))
		{
			spreadYaw = RandomFloat(-10.f, 10.f);
			spreadPitch = RandomFloat(-10.f, 10.f);
		}

		// 견착
		if (m_CamScript->GetFlag(SHOULDER))
		{
			spreadYaw = RandomFloat(-2.f, 2.f);
			spreadPitch = RandomFloat(-2.f, 2.f);
		}

		// 정조준
		if (m_CamScript->GetFlag(ADS))
		{
			spreadYaw = 0.f;
			spreadPitch = 0.f;
		}
	}

	float radYaw = XMConvertToRadians(spreadYaw);
	float radPitch = XMConvertToRadians(spreadPitch);

	Matrix spreadRot = XMMatrixRotationRollPitchYaw(radPitch, radYaw, 0.f);
	vFinalDir = XMVector3TransformNormal(vFinalDir, spreadRot);
	vFinalDir.Normalize();

	// 발사 딜레이를 넘어서면 총알을 발사한다.
	if (!m_bAuto)
	{
		m_CurRounds -= 1;

		// 풀에서 꺼내기
		CGameObject* go = CObjectPoolMgr::GetInst()->GetPoolObject(L"9mm", 10);

		// 변형값 세팅
		go->Transform()->SetRelativePos(vRayPos);

		// 스크립트에 값 전달
		MissileProjectile* BulletScript = static_cast<MissileProjectile*>(GetScriptWithType(go, SCRIPT_TYPE::MISSILESCRIPT));
		BulletScript->SetDir(vFinalDir);
		BulletScript->SetSpeed(m_InitFirePower);
		BulletScript->SetBulletInfo(m_EquippedOwner,GetOwner(), m_BulletDmg);
		BulletScript->SetVelocity(vFinalDir * m_InitFirePower);

		// 사운드 재생
		// vSpawnPos에 재생, 1번 재생, 중복재생 허용(Asset자체에서), 중복 재생 허용(Mgr자체에서), id넘기기(같은 사운드를 여러번 쓸거니 -1만넘김)
		m_AkSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_AkSound, vRayPos, 1.f, 10000.f, 1, 1.f, true, true, -1);


		// 단발이라면 한발을 소모하고 사격을 비활성화 한다.
		m_CamScript->ApplyRecoil();
		StopFiring();
	}
	else
	{
		if (m_FireDelay < m_AccTime_Fire)
		{
			m_AccTime_Fire = 0.f;
			m_CurRounds -= 1;

			// 풀에서 꺼내기
			CGameObject* go = CObjectPoolMgr::GetInst()->GetPoolObject(L"9mm", 10);

			// 변형값 세팅
			go->Transform()->SetRelativePos(vRayPos);

			// 스크립트에 값 전달
			MissileProjectile* BulletScript = static_cast<MissileProjectile*>(GetScriptWithType(go, SCRIPT_TYPE::MISSILESCRIPT));
			BulletScript->SetDir(vFinalDir);
			BulletScript->SetSpeed(m_InitFirePower);
			BulletScript->SetBulletInfo(m_EquippedOwner, GetOwner() , m_BulletDmg);
			BulletScript->SetVelocity(vFinalDir* m_InitFirePower);

			// 사운드 재생
			// vSpawnPos에 재생, 1번 재생, 중복재생 허용(Asset자체에서), 중복 재생 허용(Mgr자체에서), id넘기기(같은 사운드를 여러번 쓸거니 -1만넘김)
			m_AkSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_AkSound, vRayPos, 1.f, 10000.f, 1, 1.f, true, true, -1);
		}
	}



}

void GunController::Reload()
{

	int iLeftRounds = 0;

	if (m_bEnemy)
	{
		iLeftRounds = 90;
	}
	// 플레이어의 인벤토리에서 남아있는 총알 정보를 가져온다.
	else
	{
		iLeftRounds = m_InventoryScript->GetItemCount(m_WeaponRoundType);
	}


	// 여분의탄창이 없다면
	if (iLeftRounds == 0)
	{
		m_bReload = false;
		return;
	}

	m_AccTime_Reload += DT;

	// 사격중에 장전으로 넘어온 경우 사격을 비활성화 해준다.
	m_bFire = false;
	bool bEmptyReload = false;

	if (m_CurRounds == 0)
	{
		bEmptyReload = true;
	}
	else
	{
		bEmptyReload = false;
	}


	int iFilledRounds = 0;

	if (!m_bEnemy)
	{
		// ReloadUI : Reload Delay UI
		m_ReloadUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, 1.f - (m_ReloadingTime - m_AccTime_Reload) / m_ReloadingTime);

		// 재장전 사운드 재생
		if (bEmptyReload)
		{
			m_EmptyReloadSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_EmptyReloadSound, Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, false, false, m_EmptyReloadSoundIdx);
		}
		else
		{
			m_ReloadSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_ReloadSound, Transform()->GetRelativePos(), 1.f, 10000.f, 1, 1.f, false, false, m_ReloadSoundIdx);
		}
		

		// 남은 시간 글씨 출력
		wchar_t text[4]{};	// 3글자 출력
		if (m_ReloadingTime - m_AccTime_Reload > 0.f)
		{
			swprintf_s(text, L"%.1f", m_ReloadingTime - m_AccTime_Reload);
		}			
		m_ReloadUI->UI()->GetTextInfoRef()[0].Text = text;
	}

	// 장전 시간이 지나면
	if (m_ReloadingTime < m_AccTime_Reload)
	{
		// 여분의 탄창이 모두 있다면
		if (m_MaxRounds < iLeftRounds)
		{
			// 현재 총알에서 부족한 만큼을 계산한다.
			iFilledRounds = m_MaxRounds - m_CurRounds;
			m_CurRounds = m_MaxRounds;
		}
		else // 여분의 탄창이 부족하다면
		{
			int ineedRounds = m_MaxRounds - m_CurRounds;

			// 채워야 하는 총알보다 여분 총알이 더적다
			if (iLeftRounds < ineedRounds)
			{
				iFilledRounds = iLeftRounds;
				m_CurRounds += iFilledRounds;
			}
			// 채워야하는 총알만큼 있다.
			else
			{
				iFilledRounds = ineedRounds;
				m_CurRounds += iFilledRounds;
			}
		}

		// 남은 총알 update
		if (m_bEnemy)
		{
			iLeftRounds -= iFilledRounds;
		}			
		else
		{
			m_InventoryScript->UseItem(m_WeaponRoundType, iFilledRounds);
		}
			
		m_AccTime_Reload = 0.f;
		m_bReload = false;
		SetObjectActive(m_ReloadUI, false);
		m_PlayerScript->SetReloading(false);
		m_PlayerScript->SetReloadingEnd(true);

		if (!m_bEnemy)
		{
			// 상태
			m_PlayerScript->SetActionState(ACTION_STATE::NONE);
		}
	}

}

void GunController::StopFiring()
{
	m_bFire = false;

	if (!m_bEnemy)
	{
		m_PlayerScript->SetShot(false);

		// 상태
		m_PlayerScript->SetActionState(ACTION_STATE::NONE);
	}

	ClearKey();
}


