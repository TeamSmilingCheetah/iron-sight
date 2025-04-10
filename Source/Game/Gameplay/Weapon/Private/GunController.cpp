#include "pch.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"

#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"

#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Projectile/Public/MissileProjectile.h"

GunController::GunController()
	: WeaponController(SCRIPT_TYPE::GUNSCRIPT)
	, m_AkSoundIdx(-1)
	, m_HorizontalRecoilPower(0.f)
	, m_VerticalRecoilPower(0.f)
	, m_FireDelay(0.f)
	, m_InitFirePower(0.f)
	, m_bFire(false)
{

	// 무기 종류에 따라 변수 값 설정
	m_VerticalRecoilPower = 1.2f;
	m_HorizontalRecoilPower = 0.5f;

	m_InitFirePower = 20000.f;

	m_FireDelay = 0.1f;
}

GunController::~GunController()
{
}


void GunController::Begin()
{
	m_AkSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\ak_reverb.wav", L"Sound\\ak_reverb.wav");
}

void GunController::Tick()
{
	// 소유주가 있다면 위치를 0으로 초기화
	if (m_EquippedOwner != nullptr)
	{
		Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
	}


	// 총알을 발사한다.
	if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::PRESSED)
	{
		m_bFire = true;
	}

	// 총알 발사가 끝난다.
	if (m_CurKey == KEY::LBTN && m_CurKeyState == KEY_STATE::RELEASED)
	{
		m_bFire = false;
	}

	if (m_bFire)
	{
		Firing();
	}
}

void GunController::Firing()
{
	// 총기 애니메이션 재생



	// 총알 생성

	// Camera의 줌 여부를 확인한다.
	CGameObject* pCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
	CameraController* pCameraScript = static_cast<CameraController*>(pCamera->GetScripts()[0]);

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


	// 생성할 총알 Prefab 정보 로드
	Ptr<CPrefab> BulletPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\TestBullet.pref", L"Prefab\\TestBullet.pref");
	MissileProjectile* BulletScript = static_cast<MissileProjectile*>(BulletPrefab->GetProtoObject()->GetScripts()[0]);


	// 현재 사격 자세에 따라 정확도를 부여한다.
	float spreadYaw = 0.f;
	float spreadPitch = 0.f;


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

	float radYaw = XMConvertToRadians(spreadYaw);
	float radPitch = XMConvertToRadians(spreadPitch);

	Matrix spreadRot = XMMatrixRotationRollPitchYaw(radPitch, radYaw, 0.f);
	vFinalDir = XMVector3TransformNormal(vFinalDir, spreadRot);
	vFinalDir.Normalize();

	// 총알이 나아갈 방향을 정해준다.
	BulletScript->SetDir(vFinalDir);
	BulletScript->SetSpeed(m_InitFirePower);

	m_AccTime += DT;

	// 발사 딜레이를 넘어서면 총알을 발사한다.
	if (m_FireDelay < m_AccTime)
	{
		m_AccTime = 0.f;
		Instantiate(BulletPrefab, vSpawnPos, 0);

		// 사운드 재생
		// vSpawnPos에 재생, 1번 재생, 중복재생 허용(Asset자체에서), 중복 재생 허용(Mgr자체에서), id넘기기(같은 사운드를 여러번 쓸거니 -1만넘김) 
		m_AkSoundIdx = CSoundMgr::GetInst()->Play3DSound(m_AkSound, vSpawnPos, 1.f, 10000.f, 1, 1.f, true, true, -1);
	}

}
