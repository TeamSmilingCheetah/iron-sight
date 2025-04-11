#include "pch.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Rendering/Device/CDevice.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"

#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Weapon/Public/WeaponController.h"


PlayerCharacter::PlayerCharacter()
	: CScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT))
	, m_PaperBurnIntence(0.f)
	, m_MouseSensitivity(10.f)
	, m_Force(0.f)
	, m_Velocity(0.f)
	, m_GravidyVelocity(0.f)
	, m_Mass(3.f)
	, m_Friction(100.f)
	, m_MaxSpeed(10.f)
	, m_GravityAccel(10.f)
	, m_GravityMaxSpeed(30.f)
	, m_JumpPower(5.f)
	, m_IsGround(true)
	, m_bShoot(false)
{
	AddScriptParam(tScriptParam{SCRIPT_PARAM::FLOAT, "Player Mass", &m_Mass });				// 질량
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "Friction", &m_Friction });	// 마찰계수
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "MaxSpeed", &m_MaxSpeed });	// 최고속도
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "GravityAccel", &m_GravityAccel });	// 중력 가속도
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "GravityMaxSpeed", &m_GravityMaxSpeed });	// 중력 최대속도

	//AddScriptParam(tScriptParam{SCRIPT_PARAM::TEXTURE, "Test Texture", &m_TargetTex});
	//AddScriptParam(tScriptParam{SCRIPT_PARAM::PREFAB, "Missile", &m_Prefab});

	m_vecWeaponSlot.resize(5);
	char name[10]{};
	for (int i = 0; i < static_cast<int>(m_vecWeaponSlot.size()); ++i)
	{
		sprintf_s(name, "Slot %d", i+1);
		AddScriptParam(tScriptParam{ SCRIPT_PARAM::GAMEOBJECT, name, &m_vecWeaponSlot[i] });
	}
}

PlayerCharacter::~PlayerCharacter()
{
}

void PlayerCharacter::Begin()
{
	//m_Prefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Tile.pref", L"Prefab\\Tile.pref");
}

void PlayerCharacter::Tick()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRotation();

	UpdatePosition();
	PlayerAttack();
	PlayerReload();


	if (KEY_PRESSED(KEY::NUM_1))
	{
		//Transform()->RotateAxis(Vec3(1.f, 1.f, 1.f), 180.f * DT);
	}

	if (KEY_PRESSED(KEY::NUMPAD_0))
	{
		Vec3 vScale = Transform()->GetRelativeScale();
		vScale.x += DT * 0.1f;
		vScale.y += DT * 0.1f;
		Transform()->SetRelativeScale(vScale);
	}

	if (KEY_PRESSED(KEY::NUMPAD_9))
	{
		DrawDebugRect(Vec4(0.f, 1.f, 0.f, 0.5f), Transform()->GetRelativePos()
					  , Vec2(200.f, 200.f), Vec3(0.f, 0.f, 0.f), true, 0.f);
	}

	// 마우스 가둠
	if (KEY_TAP(KEY::SPACE))
	{
		//DrawDebugCube(Vec4(0.f, 1.f, 0.f, 1.f), vPos, Vec3(100.f, 100.f, 100.f), Vec3(0.f, 0.f, 0.f), false, 3.f);
		//DrawDebugSphere(Vec4(0.f, 1.f, 0.f, 1.f), vPos, 100.f, true, 5.f);
		//DrawDebugLine(Vec4(0.f, 0.f, 1.f, 1.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 10000.f, 0.f), true,
		//              10.f);

		CKeyMgr::GetInst()->SetMousePos();
	}

	if (KEY_PRESSED(KEY::SPACE))
	{
		UpdateRotation();
	}

	PlayerInteractWeapon();


}


void PlayerCharacter::UpdateRotation()
{
	CGameObject* pMainCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
	CameraController* pCameraScript = static_cast<CameraController*>(pMainCamera->GetScripts()[0]);
	bool bRecover = pCameraScript->IsSearchRecover();
	bool bSearch = pCameraScript->IsSearch();
	bool bShoulder = pCameraScript->IsShoulder();
	bool bADS = pCameraScript->IsADS();
	bool bTPS = pCameraScript->IsTPS();

	// 마우스 위치를 구해온다
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();
	Vec3 vPlayerRot = Transform()->GetRelativeRotation();
	Vec3 vCameraRot = pMainCamera->Transform()->GetRelativeRotation();

	// 화면의 중앙을 구한다.
	Vec2 vResoulution = CDevice::GetInst()->GetRenderResolution();
	int centerX = static_cast<int>(vResoulution.x / 2);
	int centerY = static_cast<int>(vResoulution.y / 2);

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
		if (KEY_TAP(KEY::Z))
		{
			OriginRotY = vCameraRot.y;
		}

		if (KEY_PRESSED(KEY::Z))
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
		
		pMainCamera->Transform()->SetRelativeRotation(vCameraRot);
	}

	Transform()->SetRelativeRotation(vPlayerRot);


	// 마우스 위치를 중앙으로 다시 초기화한다.
	CKeyMgr::GetInst()->SetMousePos();
	Vec3 RayDir = Vec3(0.f, 0.f, 0.f);
	float angle = vCameraRot.x * (XM_PI / 180.f);
	RayDir.y = -sin(angle);
	RayDir.z = fabs(RayDir.y) - 1;
	ColliderRay()->SetRayDir(RayDir);
}

void PlayerCharacter::PlayerReload()
{

	if (KEY_TAP(KEY::R))
	{
		// 현재 무기 슬롯이 총이라면
		if (m_CurWeaponIdx != 0 && m_CurWeaponIdx < 4)
		{			
			WeaponController* pGunScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
			pGunScript->SetCurKey(KEY::R);
			pGunScript->SetCurKeyState(KEY_STATE::TAP);
		}
	}
}

void PlayerCharacter::PlayerAttack()
{
	CGameObject* pMainCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
	CameraController* pCameraScript = static_cast<CameraController*>(pMainCamera->GetScripts()[0]);

	bool bSearch = pCameraScript->IsSearch();

	if (!bSearch)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			// 현재 무기 슬롯이 총이라면
			if (m_CurWeaponIdx != 0 && m_CurWeaponIdx < 4)
			{
				WeaponController* pGunScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
				pGunScript->SetCurKey(KEY::LBTN);
				pGunScript->SetCurKeyState(KEY_STATE::TAP);
			}			
		}
	
		if (KEY_PRESSED(KEY::LBTN))
		{
			// 총을 사용한다.
			if (m_bShoot)
			{
				WeaponController* pGunScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
				pGunScript->SetCurKey(KEY::LBTN);
				pGunScript->SetCurKeyState(KEY_STATE::PRESSED);
			}
		}
	
		if (KEY_RELEASED(KEY::LBTN))
		{
			if (m_bShoot)
			{
				m_bShoot = false;
				WeaponController* pGunScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
				pGunScript->SetCurKey(KEY::LBTN);
				pGunScript->SetCurKeyState(KEY_STATE::RELEASED);
			}
		}
	}
}

void PlayerCharacter::PlayerInteractWeapon()
{
	// 무기 교체
	assert(m_vecWeaponSlot.size() <= 5); // TEST : 무기 일단 5개 이하로 제한
	for (int i = 0; i < static_cast<int>(m_vecWeaponSlot.size()); ++i)
	{
		if (!m_vecWeaponSlot[i])
			continue;

		if (KEY_TAP(static_cast<KEY>(static_cast<int>(KEY::NUM_1) + i)))
		{
			// 해당 슬롯 활성화
			m_vecWeaponSlot[i]->SetActive(true);
			m_CurWeapon = m_vecWeaponSlot[i];
			m_CurWeaponIdx = i + 1;

			// 나머지 슬롯은 비활성화
			for (int j = 0; j < static_cast<int>(m_vecWeaponSlot.size()); ++j)
			{
				if (!m_vecWeaponSlot[j] || j == i)
					continue;

				m_vecWeaponSlot[j]->SetActive(false);

			}

			break;
		}
	}

	// 모든 무기 내리기 (무기 미착용 상태로)
	if (KEY_TAP(KEY::B))
	{
		// 모든 무기 슬롯 비활성화
		for (int i = 0; i < static_cast<int>(m_vecWeaponSlot.size()); ++i)
		{
			if (!m_vecWeaponSlot[i])
				continue;
			m_vecWeaponSlot[i]->SetActive(false);
		}
		m_CurWeapon = nullptr;
		m_CurWeaponIdx = 0;
	}

	// 현재 들고 있는 무기를 버린다.
	if (KEY_TAP(KEY::G))
	{
		if (m_CurWeapon != nullptr)
		{
			Vec3 vPlayerPos = Transform()->GetRelativePos();

			// 부모를 없는 독립 개체로 바꿔준다.
			AddChild(nullptr, m_CurWeapon);
			// 본래 Layer로 변경해준다.
			m_CurWeapon->SetLayerIdx(0);

			// 소유주를 삭제하고, 현재 Player 위치에 무기를 다시 생성시킨다.
			WeaponController* pGunScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
			pGunScript->SetEquippedOwner(nullptr);
			Vec3 vWeaponPos = vPlayerPos;
			vWeaponPos.y += 400.f;
			m_CurWeapon->Transform()->SetRelativePos(vWeaponPos);

			// Player의 무기정보를 비워준다. 
			m_vecWeaponSlot[m_CurWeaponIdx - 1] = nullptr;
			m_CurWeapon = nullptr;
			m_CurWeaponIdx = 0;
		}
	}

}

void PlayerCharacter::SaveComponent(FILE* _File)
{
	//fwrite(&m_PlayerSpeed, sizeof(float), 1, _File);
	fwrite(&m_PaperBurnIntence, sizeof(float), 1, _File);
	SaveAssetRef(m_TargetTex, _File);
	UINT slotCount = static_cast<UINT>(m_vecWeaponSlot.size());
	fwrite(&slotCount, sizeof(UINT), 1, _File);
	for (UINT i = 0; i < slotCount; ++i)
	{
		SaveObjectRef(m_vecWeaponSlot[i], _File);
	}
}

void PlayerCharacter::LoadComponent(FILE* _File)
{
	//fread(&m_PlayerSpeed, sizeof(float), 1, _File);
	fread(&m_PaperBurnIntence, sizeof(float), 1, _File);
	LoadAssetRef(m_TargetTex, _File);
	UINT slotCount = 0;
	fread(&slotCount, sizeof(UINT), 1, _File);
	m_vecWeaponSlot.resize(slotCount);
	for (UINT i = 0; i < slotCount; ++i)
	{
		LoadObjectRef(m_vecWeaponSlot[i], _File);
	}
}
