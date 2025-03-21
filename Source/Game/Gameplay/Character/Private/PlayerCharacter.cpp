#include "pch.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Rendering/Device/CDevice.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"

#include "Game/Gameplay/Character/Public/CameraController.h"

PlayerCharacter::PlayerCharacter()
	: CScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT))
	, m_PlayerSpeed(1500)
	, m_PaperBurnIntence(0.f)
	, m_MouseSensitivity(10.f)
	, m_Acceleration(50.f)
	, m_Deceleration(450.f)
	, m_MaxSpeed(10.f)
{
	AddScriptParam(tScriptParam{SCRIPT_PARAM::FLOAT, "Player Speed", &m_PlayerSpeed});
	AddScriptParam(tScriptParam{SCRIPT_PARAM::TEXTURE, "Test Texture", &m_TargetTex});
	AddScriptParam(tScriptParam{SCRIPT_PARAM::PREFAB, "Missile", &m_Prefab});

	m_vecWeaponSlot.resize(7);
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

	if (KEY_PRESSED(KEY::NUM_1))
	{
		Transform()->RotateAxis(Vec3(1.f, 1.f, 1.f), 180.f * DT);
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

	// 미사일 발사
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

	// 무기 교체
	assert(m_vecWeaponSlot.size() <= 7); // TEST : 무기 일단 7개 이하로 제한
	for (int i = 0; i < static_cast<int>(m_vecWeaponSlot.size()); ++i)
	{
		if (!m_vecWeaponSlot[i])
			continue;

		if (KEY_TAP(static_cast<KEY>(static_cast<int>(KEY::NUM_1) + i)))
		{
			// 해당 슬롯 활성화
			m_vecWeaponSlot[i]->SetActive(true);

			// 나머지 슬롯은 비활성화
			for (int j = 0; j < static_cast<int>(m_vecWeaponSlot.size()); ++j)
			{
				if (!m_vecWeaponSlot[j] || j==i)
					continue;

				m_vecWeaponSlot[j]->SetActive(false);
			}

			break;
		}
	}
	
}

void PlayerCharacter::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject,
								 CCollider2D* _OtherCollider)
{
}

void PlayerCharacter::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject,
							CCollider2D* _OtherCollider)
{
}

void PlayerCharacter::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject,
							   CCollider2D* _OtherCollider)
{
}

void PlayerCharacter::UpdatePosition()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRotation();
	float radian = vRot.y * XM_PI / 180.f;

	Vec3 vFowardDir = { -sinf(radian), 0.f, -cosf(radian) };
	Vec3 vRightDir = { -cosf(radian), 0.f, sinf(radian) };
	Vec3 vInputDir = { 0.f,0.f,0.f };

	if (KEY_PRESSED(KEY::LSHIFT) &&
		KEY_PRESSED(KEY::W) &&
		!(KEY_PRESSED(KEY::A)) &&
		!(KEY_PRESSED(KEY::S)) &&
		!(KEY_PRESSED(KEY::D)))
	{
		m_Acceleration = 60.f;
		m_MaxSpeed = 20.f;
	}
	else
	{
		m_Acceleration = 50.f;
		m_MaxSpeed = 10.f;
	}

	// 해당하는 방향으로 벡터를 추가한다.
	if (KEY_PRESSED(KEY::W))
	{
		vInputDir += vFowardDir;
	}
	if (KEY_PRESSED(KEY::A))
	{
		vInputDir += -vRightDir;
	}
	if (KEY_PRESSED(KEY::S))
	{
		vInputDir += -vFowardDir;
	}
	if (KEY_PRESSED(KEY::D))
	{
		vInputDir += vRightDir;
	}

	// 입력이 있는 경우
	if (vInputDir.Length() > 0.f)
	{
		// 방향 정규화
		vInputDir.Normalize();

		// 저장된 방향으로 가속도만큼 현재 속도를 증가시킨다.
		m_velocity += vInputDir * m_Acceleration * DT;

		// 현재의 방향을 즉시 입력 방향으로 맞춰준다.
		float currentSpeed = m_velocity.Length();

		// 기존의 속도를 현재 바라보는 방향으로 적용해줌
		if (currentSpeed > 0.f)
		{
			m_velocity = vInputDir * currentSpeed;
		}
		else
		{
			m_velocity = vInputDir * 0.f;
		}

		// 최대 속도를 넘어가면 제한해준다.
		float newSpeed = m_velocity.Length();
		if (newSpeed > m_MaxSpeed)
		{
			m_velocity.Normalize();
			m_velocity = m_velocity * m_MaxSpeed;
		}
	}
	// 입력이 없을 경우
	else
	{
		float currentSpeed = m_velocity.Length();
		// 현재 속도가 있다면 감속해준다.
		if (currentSpeed > 0.f)
		{
			float newSpeed = currentSpeed - m_Deceleration * DT;
			if (newSpeed < 0.f)
			{
				newSpeed = 0.f;
			}
			m_velocity.Normalize();
			m_velocity = (newSpeed > 0.f) ? m_velocity * newSpeed : Vec3{ 0.f,0.f,0.f };
			//m_velocity = m_velocity * newSpeed;
		}
	}

	vPos += m_velocity;
	Transform()->SetRelativePos(vPos);
}

void PlayerCharacter::UpdateRotation()
{
	CGameObject* pMainCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
	CameraController* pCameraScript = static_cast<CameraController*>(pMainCamera->GetScripts()[0]);
	bool bRecover = pCameraScript->IsSearchRecover();

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
	if (KEY_TAP(KEY::Z))
	{
		OriginRotY = vCameraRot.y;
	}

	// 해당 변화값으로 Player 회전 적용
	if (KEY_PRESSED(KEY::Z))
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
	else
	{
		if (!bRecover)
		{
			vPlayerRot.y += deltaX * m_MouseSensitivity * DT;
		}			
	}

	// 화면을 위아래로 회전시킨다.
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
}

void PlayerCharacter::SaveComponent(FILE* _File)
{
	fwrite(&m_PlayerSpeed, sizeof(float), 1, _File);
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
	fread(&m_PlayerSpeed, sizeof(float), 1, _File);
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
