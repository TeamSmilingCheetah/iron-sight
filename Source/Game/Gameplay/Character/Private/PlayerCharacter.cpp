#include "pch.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

PlayerCharacter::PlayerCharacter()
	: CScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT))
	  , m_PlayerSpeed(500)
	  , m_PaperBurnIntence(0.f)
{
	AddScriptParam(tScriptParam{SCRIPT_PARAM::FLOAT, "Player Speed", &m_PlayerSpeed});
	AddScriptParam(tScriptParam{SCRIPT_PARAM::TEXTURE, "Test Texture", &m_TargetTex});
	AddScriptParam(tScriptParam{SCRIPT_PARAM::PREFAB, "Missile", &m_Prefab});

	m_vecWeaponSlot.resize(7);
	char name[10]{};
	for (size_t i = 0; i < m_vecWeaponSlot.size(); ++i)
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

	if (KEY_PRESSED(KEY::LEFT))
		vPos.x -= m_PlayerSpeed * DT;
	if (KEY_PRESSED(KEY::RIGHT))
		vPos.x += m_PlayerSpeed * DT;
	if (KEY_PRESSED(KEY::UP))
		vPos.y += m_PlayerSpeed * DT;
	if (KEY_PRESSED(KEY::DOWN))
		vPos.y -= m_PlayerSpeed * DT;

	if (KEY_PRESSED(KEY::Z))
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

	Transform()->SetRelativePos(vPos);

	// 미사일 발사
	if (KEY_TAP(KEY::SPACE))
	{
		//DrawDebugCube(Vec4(0.f, 1.f, 0.f, 1.f), vPos, Vec3(100.f, 100.f, 100.f), Vec3(0.f, 0.f, 0.f), false, 3.f);
		//DrawDebugSphere(Vec4(0.f, 1.f, 0.f, 1.f), vPos, 100.f, true, 5.f);
		DrawDebugLine(Vec4(0.f, 0.f, 1.f, 1.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 10000.f, 0.f), true,
					  10.f);
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
	/*UINT slotCount = 0;
	fread(&slotCount, sizeof(UINT), 1, _File);
	m_vecWeaponSlot.resize(slotCount);
	for (UINT i = 0; i < slotCount; ++i)
	{
		LoadObjectRef(m_vecWeaponSlot[i], _File);
	}*/
}
