#include "pch.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"

#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"
#include "Engine/System/Public/Manager/CObjectPoolMgr.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Projectile/Public/MissileProjectile.h"

GunController::GunController()
	: WeaponController(SCRIPT_TYPE::GUNSCRIPT)
	, m_AkSoundIdx(-1)
	, m_HorizontalRecoilPower(0.f)
	, m_VerticalRecoilPower(0.f)
	, m_FireDelay(0.f)
	, m_InitFirePower(0.f)
	, m_AccTime_Fire(0.f)
	, m_AccTime_Reload(0.f)
	, m_MaxRounds(0)
	, m_CurRounds(0)
	, m_bFire(false)
	, m_bReload(false)
{

	// 무기 종류에 따라 변수 값 설정
	m_VerticalRecoilPower = 1.2f;
	m_HorizontalRecoilPower = 0.5f;

	m_InitFirePower = 20000.f;

	m_FireDelay = 0.1f;
	m_MaxRounds = 30;

	m_ReloadingTime = 3.f;
}

GunController::~GunController()
{
}


void GunController::Begin()
{
	WeaponController::Begin();
	m_AkSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\ak_reverb.wav", L"Sound\\ak_reverb.wav");

}

void GunController::Tick()
{
	// 소유주가 없다면 return
	if (m_EquippedOwner == nullptr || m_bIsEquipped == false)
	{
		return;
	}

	// 플레이어일 시 처리
	if (!m_bEnemy)
	{
		PlayerCharacter* pPlayerScript = nullptr;

		// 소유주가 있다면 위치를 0으로 초기화
		if (m_EquippedOwner != nullptr)
		{
			pPlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_EquippedOwner, SCRIPT_TYPE::PLAYERSCRIPT));
		}


		// 총알을 발사한다.
		if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::TAP)
		{
			if (0 < m_CurRounds && !m_bReload)
			{
				pPlayerScript->SetShot(true);
			}
		}

		if (m_CurKey == KEY::R && m_CurKeyState == KEY_STATE::TAP && m_CurRounds != m_MaxRounds)
		{
			if (!m_bReload && m_CurRounds != m_MaxRounds)
			{
				pPlayerScript->SetShot(false);
				m_bReload = true;
			}

		}
	}
	else
	{
		if (m_CurKey == KEY::R && m_CurKeyState == KEY_STATE::TAP && m_CurRounds != m_MaxRounds)
		{
			if (!m_bReload && m_CurRounds != m_MaxRounds)
			{
				m_bReload = true;
			}

		}
	}


	// 총알을 발사한다.
	if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::PRESSED)
	{
		if (0 < m_CurRounds)
			m_bFire = true;
	}

	// 총알 발사가 끝난다.
	if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::RELEASED)
	{
		m_bFire = false;
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

void GunController::Firing()
{
	// 총기 애니메이션 재생

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
			return;
		}

		// Camera의 줌 여부를 확인한다.
		CGameObject* pCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
		CameraController* pCameraScript = static_cast<CameraController*>(pCamera->GetScripts()[0]);

		// 지향사격
		if (!pCameraScript->IsShoulder() && !pCameraScript->IsADS())
		{
			spreadYaw = RandomFloat(-10.f, 10.f);
			spreadPitch = RandomFloat(-10.f, 10.f);
		}

		// 견착
		if (pCameraScript->IsShoulder())
		{
			spreadYaw = RandomFloat(-2.f, 2.f);
			spreadPitch = RandomFloat(-2.f, 2.f);
		}

		// 정조준
		if (pCameraScript->IsADS())
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

	m_AccTime_Fire += DT;

	// 발사 딜레이를 넘어서면 총알을 발사한다.
	if (m_FireDelay < m_AccTime_Fire)
	{
		m_AccTime_Fire = 0.f;
		m_CurRounds -= 1;

		// 풀에서 꺼내기
		CGameObject* go = CObjectPoolMgr::GetInst()->GetPoolObject(L"TestBullet", 0);

		// 변형값 세팅
		go->Transform()->SetRelativePos(vRayPos);

		// 스크립트에 값 전달
		MissileProjectile* BulletScript = static_cast<MissileProjectile*>(GetScriptWithType(go, SCRIPT_TYPE::MISSILESCRIPT));
		BulletScript->SetDir(vFinalDir);
		BulletScript->SetSpeed(m_InitFirePower);

		// 사운드 재생
		// vSpawnPos에 재생, 1번 재생, 중복재생 허용(Asset자체에서), 중복 재생 허용(Mgr자체에서), id넘기기(같은 사운드를 여러번 쓸거니 -1만넘김)
		m_AkSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_AkSound, vRayPos, 1.f, 10000.f, 1, 1.f, true, true, -1);
	}

}

void GunController::Reload()
{

	// 플레이어의 인벤토리에서 남아있는 총알 정보를 가져온다.
	// PlayerCharacter* pPlayerScript = static_cast<PlayerCharacter*>(CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player")->GetScripts()[0]);
	static int iLeftRounds = 70; // 임시 코드

	// 여분의탄창이 없다면
	if (iLeftRounds == 0)
	{
		m_bReload = false;
		return;
	}

	// 총기 애니메이션 재생


	m_AccTime_Reload += DT;

	// 사격중에 장전으로 넘어온 경우 사격을 비활성화 해준다.
	m_bFire = false;

	int iFilledRounds = 0;
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
		iLeftRounds -= iFilledRounds;
		m_AccTime_Reload = 0.f;
		m_bReload = false;
	}
}
