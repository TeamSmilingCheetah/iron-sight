#include "pch.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Rendering/Device/CDevice.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"

#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Inventory/Public/Item.h"


PlayerCharacter::PlayerCharacter()
	: CScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT))
	, m_MainCamera(nullptr)
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
	, m_CurWeaponIdx(NONE_WEAPON)
	, m_CurWeapon(nullptr)
	, m_IsGround(true)
	, m_bShoot(false)
	, m_bCanThrow(false)
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
		AddScriptParam(tScriptParam{ SCRIPT_PARAM::GAMEOBJECT, name, &m_vecWeaponSlot[i].Object });
	}
}

PlayerCharacter::~PlayerCharacter()
{
}

void PlayerCharacter::Begin()
{
	m_MainCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
	//m_Prefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Tile.pref", L"Prefab\\Tile.pref");
}

void PlayerCharacter::Tick()
{

	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRotation();

	UpdatePosition();
	PlayerAttack();
	PlayerReload();


	if (KEY_PRESSED(KEY::NUMPAD_9))
	{
		DrawDebugRect(Vec4(0.f, 1.f, 0.f, 0.5f), Transform()->GetRelativePos()
					  , Vec2(200.f, 200.f), Vec3(0.f, 0.f, 0.f), true, 0.f);
	}

	// 마우스 가둠
	if (KEY_TAP(KEY::SPACE))
	{
		CKeyMgr::GetInst()->SetMousePos();
	}

	if (KEY_PRESSED(KEY::SPACE))
	{
		UpdateRotation();
	}

	PlayerInteractWeapon();

	if (m_bThrowBoom)
	{
		if (THROWABLE_FIRST <= m_CurWeaponIdx || m_CurWeaponIdx <= THROWABLE_SECOND)
		{
			m_CurWeaponIdx = NONE_WEAPON;
			m_CurWeapon = nullptr;
			m_bThrowBoom = false;
		}
	}
}


void PlayerCharacter::UpdateRotation()
{
	CameraController* pCameraScript = static_cast<CameraController*>(m_MainCamera->GetScripts()[0]);
	bool bRecover = pCameraScript->IsSearchRecover();
	bool bSearch = pCameraScript->IsSearch();
	bool bShoulder = pCameraScript->IsShoulder();
	bool bADS = pCameraScript->IsADS();
	bool bTPS = pCameraScript->IsTPS();

	// 마우스 위치를 구해온다
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();
	Vec3 vPlayerRot = Transform()->GetRelativeRotation();
	Vec3 vCameraRot = m_MainCamera->Transform()->GetRelativeRotation();

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
		
		m_MainCamera->Transform()->SetRelativeRotation(vCameraRot);
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
		if (m_CurWeaponIdx <= SECONDARY_FIRST)
		{			
			WeaponController* pGunScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
			pGunScript->SetCurKey(KEY::R);
			pGunScript->SetCurKeyState(KEY_STATE::TAP);
		}
	}
}

void PlayerCharacter::PlayerAttack()
{
	CameraController* pCameraScript = static_cast<CameraController*>(m_MainCamera->GetScripts()[0]);

	bool bSearch = pCameraScript->IsSearch();

	if (!bSearch)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			// 현재 무기 슬롯이 총이라면
			if (m_CurWeaponIdx <= THROWABLE_SECOND)
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

			if (m_bCanThrow)
			{
				if (KEY_TAP(KEY::R))
				{
					WeaponController* pWeaponScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
					pWeaponScript->SetCurKey(KEY::R);
					pWeaponScript->SetCurKeyState(KEY_STATE::TAP);
				}				
			}
		}
	
		if (KEY_RELEASED(KEY::LBTN))
		{
			// 총기
			if (m_bShoot)
			{
				m_bShoot = false;
				WeaponController* pGunScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
				pGunScript->SetCurKey(KEY::LBTN);
				pGunScript->SetCurKeyState(KEY_STATE::RELEASED);
			}

			// 투척무기
			if (m_bCanThrow)
			{
				if (m_CurWeapon != nullptr)
				{
					WeaponController* pWeaponScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
					pWeaponScript->SetCurKey(KEY::LBTN);
					pWeaponScript->SetCurKeyState(KEY_STATE::RELEASED);

					// 인벤토리에서 투척무기 하나 제거
					// TODO : 개선
					ITEM_TYPE type = static_cast<ItemScript*>(m_vecWeaponSlot[m_CurWeaponIdx].Object->GetScript(ITEMSCRIPT))->GetItemType();
					InventoryController* inventory = static_cast<InventoryController*>(GetOwner()->GetChildByName(L"Inventory")->GetScript(INVENTORYSCRIPT));
					
					// 아이템이 남았다면
					if (inventory->UseItem(type, 1))
					{
						// TODO: object pooling으로 개선
						Ptr<CPrefab> pPrefab = ItemMgr::GetInst()->GetItemInfo(type).Prefab;
						m_CurWeapon = m_vecWeaponSlot[m_CurWeaponIdx].Object = pPrefab->Instantiate();
						static_cast<WeaponController*>(m_CurWeapon->GetScript(THROWABLESCRIPT))->SetEquippedOwner(GetOwner());

						CreateObject(m_CurWeapon, 0, false);
						AttachItem(m_CurWeapon, GetPlayeChildMeshObject(L"hand_r"), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
					}

					// 아이템이 남지 않았다면
					else
					{
						m_vecWeaponSlot[m_CurWeaponIdx].Object = nullptr;
						m_vecWeaponSlot[m_CurWeaponIdx].Type = ITEM_TYPE::END;

						m_bCanThrow = false;
						m_CurWeapon = nullptr;
						m_CurWeaponIdx = NONE_WEAPON;
					}
				}				
			}
		}

		if (KEY_TAP(KEY::RBTN))
		{
			// 투척무기이면
			if (THROWABLE_FIRST <= m_CurWeaponIdx && m_CurWeaponIdx <= THROWABLE_SECOND)
			{
				WeaponController* pGunScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
				pGunScript->SetCurKey(KEY::RBTN);
				pGunScript->SetCurKeyState(KEY_STATE::TAP);
			}
		}

		if (KEY_PRESSED(KEY::RBTN))
		{
			if (m_bCanThrow)
			{
				if (KEY_TAP(KEY::R))
				{
					WeaponController* pWeaponScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
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
				if (m_CurWeapon != nullptr)
				{
					m_bCanThrow = false;
					WeaponController* pWeaponScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
					pWeaponScript->SetCurKey(KEY::RBTN);
					pWeaponScript->SetCurKeyState(KEY_STATE::RELEASED);
					m_vecWeaponSlot[m_CurWeaponIdx].Object = nullptr;
					m_vecWeaponSlot[m_CurWeaponIdx].Type = ITEM_TYPE::END;
					m_CurWeapon = nullptr;
					m_CurWeaponIdx = NONE_WEAPON;
				}
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
		if (!m_vecWeaponSlot[i].Object)
			continue;

		if (KEY_TAP(static_cast<KEY>(static_cast<int>(KEY::NUM_1) + i)))
		{
			// 해당 슬롯 활성화
			SetObjectActive(m_vecWeaponSlot[i].Object, true);
			m_CurWeapon = m_vecWeaponSlot[i].Object;
			m_CurWeaponIdx = i;
			wstring str = L"hand_r";
			AttachItem(m_CurWeapon, GetPlayeChildMeshObject(str), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
			WeaponController* pWeaponScript = static_cast<WeaponController*>(m_CurWeapon->GetScripts()[0]);
			pWeaponScript->SetEquip(true);

			// 나머지 슬롯은 비활성화
			for (int j = 0; j < static_cast<int>(m_vecWeaponSlot.size()); ++j)
			{
				if (!m_vecWeaponSlot[j].Object || j == i)
					continue;

				// 주무기는 비활성화가 아닌 등에 다시 부착 시켜 준다.
				if (j == PRIMARY_FIRST)
				{
					wstring str = L"coat_b_04";
					AttachItem(m_vecWeaponSlot[j].Object, GetPlayeChildMeshObject(str), Vec3(-810.f, -99.f, -234.f), Vec3(75.9f, -2.f, -4.3f));
				}
				else if (j == PRIMARY_SECOND)
				{
					wstring str = L"coat_b_04";
					AttachItem(m_vecWeaponSlot[j].Object, GetPlayeChildMeshObject(str), Vec3(-810.f, -99.f, 75.f), Vec3(75.9f, -2.f, -4.3f));
				}
				// 나머지는 비활성화 해준다.
				else
				{
					SetObjectActive(m_vecWeaponSlot[j].Object, false);
				}				
				WeaponController* pWeaponScript = static_cast<WeaponController*>(m_vecWeaponSlot[j].Object->GetScripts()[0]);
				pWeaponScript->SetEquip(false);
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
			if (!m_vecWeaponSlot[i].Object)
				continue;

			if (i == PRIMARY_FIRST)
			{
				AttachItem(m_vecWeaponSlot[i].Object, GetPlayeChildMeshObject(L"coat_b_04"), Vec3(-810.f, -99.f, -234.f), Vec3(75.9f, -2.f, -4.3f));
				SetObjectActive(m_vecWeaponSlot[i].Object, true);
			}
			else if (i == PRIMARY_SECOND)
			{
				AttachItem(m_vecWeaponSlot[i].Object, GetPlayeChildMeshObject(L"coat_b_04"), Vec3(-810.f, -99.f, 75.f), Vec3(75.9f, -2.f, -4.3f));
				SetObjectActive(m_vecWeaponSlot[i].Object, true);
			}
			else
			{
				SetObjectActive(m_vecWeaponSlot[i].Object, false);
			}
			
		}
		m_CurWeapon = nullptr;
		m_CurWeaponIdx = NONE_WEAPON;
	}

	// 현재 들고 있는 무기를 버린다.
	if (KEY_TAP(KEY::G))
	{
		if (m_CurWeapon != nullptr)
		{
			// 인벤토리에서 아이템 개수 관리
			InventoryController* inventory = static_cast<InventoryController*>(GetOwner()->GetChildByName(L"Inventory")->GetScript(INVENTORYSCRIPT));

			inventory->DropItem(m_vecWeaponSlot[m_CurWeaponIdx].Type, 1);
		}
	}

}

CGameObject* PlayerCharacter::GetPlayeChildMeshObject(const wstring& _str)
{
	Matrix invPlayerWorld = Transform()->GetWorldInvMat();

	vector<CGameObject*> vecBones = Animator3D()->GetvecBone();
	unordered_map<wstring, CGameObject*> mapBones = Animator3D()->GetmapBone();

	auto iter = mapBones.find(_str);

	return iter->second;
}

void PlayerCharacter::EquipSlot(CGameObject* _Item)
{
	// 장착할 수 있는 아이템이라는 것이 보장되어 있음 (InventoryController)
	// TODO : 개선
	WeaponController* pScript = static_cast<WeaponController*>(_Item->GetScript(GUNSCRIPT));
	if (pScript == nullptr)
		pScript = static_cast<WeaponController*>(_Item->GetScript(THROWABLESCRIPT));

	assert(pScript != nullptr);		// WeaponScript가 있다는 가정

	WEAPON_TYPE eWeaponType = pScript->GetWeaponType();
	static bool bCanEquip = false;

	switch (eWeaponType)
	{
		// 주무기
	case WEAPON_TYPE::PRIMARY:
	{
		int slotIdx = 0;
		for (int i = PRIMARY_FIRST; i <= PRIMARY_SECOND; ++i)
		{
			if (m_vecWeaponSlot[i].Object == nullptr)
			{
				slotIdx = i;
				break;
			}
		}

		// 빈 슬롯이 없었다면 첫번째 슬롯과 교체
		if (m_vecWeaponSlot[slotIdx].Object)
		{
			// 현재 슬롯을 해제
			DetachItem(m_vecWeaponSlot[slotIdx].Object);
			InventoryController* inventory = static_cast<InventoryController*>(GetOwner()->GetChildByName(L"Inventory")->GetScript(INVENTORYSCRIPT));
			inventory->UseItem(m_vecWeaponSlot[slotIdx].Type);
		}

		m_vecWeaponSlot[slotIdx].Object = _Item;
		m_vecWeaponSlot[slotIdx].Type = static_cast<ItemScript*>(_Item->GetScript(ITEMSCRIPT))->GetItemType();

		bCanEquip = true;
		CGameObject* pBoneObj = GetPlayeChildMeshObject(L"coat_b_04");

		if (slotIdx == PRIMARY_FIRST)
		{
			AttachItem(_Item, pBoneObj, Vec3(-810.f, -99.f, -234.f), Vec3(75.9f, -2.f, -4.3f));
		}
		else
		{
			AttachItem(_Item, pBoneObj, Vec3(-810.f, -99.f, 75.f), Vec3(75.9f, -2.f, -4.3f));
		}

		SetObjectActive(_Item, true);
	}
		break;
		// 보조무기
	case WEAPON_TYPE::SECONDARY:
		if (m_vecWeaponSlot[SECONDARY_FIRST].Object == nullptr)
		{
			m_vecWeaponSlot[SECONDARY_FIRST].Object = _Item;
			m_vecWeaponSlot[SECONDARY_FIRST].Type = static_cast<ItemScript*>(_Item->GetScript(ITEMSCRIPT))->GetItemType();
			AddChild(GetOwner(), _Item);
			SetObjectActive(_Item, false);
			bCanEquip = true;
		}
		break;
		// 투척무기				
	case WEAPON_TYPE::THROWABLE:
	{
		for (int i = THROWABLE_FIRST; i <= THROWABLE_SECOND; ++i)
		{
			// 슬롯에 이미 차 있는 경우
			if (m_vecWeaponSlot[i].Object != nullptr)
			{
				// 같은 weapon 인 경우
				if (m_vecWeaponSlot[i].Type == static_cast<ItemScript*>(_Item->GetScript(ITEMSCRIPT))->GetItemType())
					break;

				// 다른 weapon인 경우
				continue;
			}

			m_vecWeaponSlot[i].Object = _Item;
			m_vecWeaponSlot[i].Type = static_cast<ItemScript*>(_Item->GetScript(ITEMSCRIPT))->GetItemType();
			AttachItem(_Item, GetPlayeChildMeshObject(L"hand_r"), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
			SetObjectActive(_Item, false);
			bCanEquip = true;
			break;
		}
		
	}
		break;
	default:
		break;
	}

	// 장착할 수 있는 경우 :
	// 무기 스크립트 쪽에 알려줘야 함
	if (bCanEquip)
	{
		// Player를 소유주으로 등록, 자식에 무기를 넣어준다.
		pScript->SetEquippedOwner(GetOwner());

		bCanEquip = false;
		m_bCanEquip = false;
	}

	// 장착할 수 없는 경우 :
	// 이미 해당 타입이 슬롯에 존재하거나 빈 슬롯이 없었다면 아이템 정보만 얻고 객체는 파괴함
	else
	{
		// TODO : object pooling으로 개선 (파괴가 아니라 비활성화 후 pool에 넣기)
		DestroyObject(_Item);
	}
}

void PlayerCharacter::ReleaseSlot(ITEM_TYPE _Type, int _Count)
{
	for (int i = 0; i < static_cast<int>(m_vecWeaponSlot.size()); ++i)
	{
		if (m_vecWeaponSlot[i].Type == _Type)
		{
			// 부모 관계 해제, 레이어 이동, transform 설정
			DetachItem(m_vecWeaponSlot[i].Object);

			// 개수 설정
			static_cast<ItemScript*>(m_vecWeaponSlot[i].Object->GetScript(ITEMSCRIPT))->SetCount(_Count);

			// 주인 해제
			WeaponController* pGunScript = static_cast<WeaponController*>(m_vecWeaponSlot[i].Object->GetScripts()[0]);
			pGunScript->SetEquippedOwner(nullptr);

			m_vecWeaponSlot[i].Object = nullptr;
			m_vecWeaponSlot[i].Type = ITEM_TYPE::END;

			// 현재 무기와 일치하는 경우
			if (i == m_CurWeaponIdx)
			{
				m_CurWeapon = nullptr;
				m_CurWeaponIdx = NONE_WEAPON;
			}

			return;
		}
	}
}

void PlayerCharacter::AttachItem(CGameObject* _Item, CGameObject* _BoneObject, Vec3 _RelativePos, Vec3 _RelativeRot)
{
	AddChild(_BoneObject, _Item);
	_Item->Transform()->SetRelativePos(_RelativePos);
	_Item->Transform()->SetRelativeRotation(_RelativeRot);
}

void PlayerCharacter::DetachItem(CGameObject* _Item)
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRotation();

	// 아이템 레이어로 변경
	assert(CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(6)->GetName() == L"Item");
	ChangeLayer(_Item, 6);
	SetObjectActive(_Item, true);
	AttachItem(_Item, nullptr, vPos, vRot);
}

void PlayerCharacter::SaveComponent(FILE* _File)
{
	//fwrite(&m_PlayerSpeed, sizeof(float), 1, _File);
	fwrite(&m_PaperBurnIntence, sizeof(float), 1, _File);
	SaveAssetRef(m_TargetTex, _File);
}

void PlayerCharacter::LoadComponent(FILE* _File)
{
	//fread(&m_PlayerSpeed, sizeof(float), 1, _File);
	fread(&m_PaperBurnIntence, sizeof(float), 1, _File);
	LoadAssetRef(m_TargetTex, _File);
}
