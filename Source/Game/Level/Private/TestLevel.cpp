#include "pch.h"
#include "Game/Level/Public/TestLevel.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Light/CLight3D.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"
#include "Engine/Runtime/Public/Component/Rendering/CSkyBox.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/System/Public/Manager/CCollisionMgr.h"

#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Character/Public/EnemyVisionScript.h"
#include "Game/Gameplay/Character/Public/InteractionHandler.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Character/Public/TestCharacter.h"
#include "Game/Gameplay/Door/Public/DoorScript.h"
#include "Game/GamePlay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Inventory/Public/Item.h"
#include "Game/Gameplay/Inventory/Public/UI_Inventory.h"
#include "Game/Gameplay/Inventory/Public/UI_Item.h"
#include "Game/Gameplay/Inventory/Public/UI_Vicinity.h"
#include "Game/Gameplay/UI/Public/KillinfoUIScript.h"
#include "Game/Gameplay/UI/Public/RoundsUIScript.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"
#include "Game/Gameplay/Weapon/Public/ThrowableController.h"
#include "Game/Gameplay/Weapon/Public/WeaponController.h"

void TestLevel::CreateTestLevel()
{
	CLevel* LevelForTest = new CLevel;

	// 테스트 레벨을 현재 레벨로 지정
	ChangeLevel(LevelForTest, LEVEL_STATE::STOP);

	SetUpLayer(LevelForTest);
	SetUpCollision(LevelForTest);
	SetUpCamera(LevelForTest);
	SetUpLight(LevelForTest);
	SetUpSkyBox(LevelForTest);
	SetUpLandscape(LevelForTest);

	// Initialize Item Parts
	auto UIInfo = SetUpUI(LevelForTest);
	ItemMgr::GetInst()->Init();

	SetUpPlayer(LevelForTest, UIInfo);

	// Smoke
	SetupFBX(
	LevelForTest,
	L"FBX\\Props\\Throwable\\M18_Smoke.fbx",
	L"Smoke Grenade",
	Vec3(0.f, 0.f, 10000.f),
	Vec3(15.f, 15.f, 15.f),
	Vec3(0.f, 0.f, 0.f),
	{
		[](CGameObject* obj) {
			obj->AddComponent(new CCollider3D);
			obj->Collider3D()->SetName(L"Weapon");
			obj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
			obj->Collider3D()->SetIndependentScale(true);
			obj->Collider3D()->SetTrigger(true);
		},
		[](CGameObject* obj) {
			obj->AddComponent(new ThrowableController);
			WeaponController* script = static_cast<WeaponController*>(obj->GetScripts()[0]);
			script->SetWeaponType(WEAPON_TYPE::THROWABLE);
		},
		[](CGameObject* obj) {
			obj->AddComponent(new ItemScript(ITEM_TYPE::SMOKEBOMB));
		}
	},
	6,
	false
	);

	// Test Primary Weapon
	SetupFBX(
	LevelForTest,
	L"FBX\\Props\\Heal\\Energy Drink.fbx",
	L"Energy Drink",
	Vec3(0.f, 0.f, 1000.f),
	Vec3(500.f, 500.f, 500.f),
	Vec3(0.f, 0.f, 0.f),
	{
		[](CGameObject* obj) {
			obj->AddComponent(new CCollider3D);
			obj->Collider3D()->SetName(L"Weapon");
			obj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
			obj->Collider3D()->SetIndependentScale(true);
			obj->Collider3D()->SetTrigger(true);
		},
		[](CGameObject* obj) {
			obj->AddComponent(new GunController);
			WeaponController* script = static_cast<WeaponController*>(obj->GetScripts()[0]);
			script->SetWeaponType(WEAPON_TYPE::SECONDARY);
		},
		[](CGameObject* obj) {
			obj->AddComponent(new ItemScript(ITEM_TYPE::ENERGY_DRINK));
		}
	},
	6,
	false
	);

	// Test Secondary Weapon
	SetupFBX(
	LevelForTest,
	L"FBX\\Props\\Heal\\First Aid Kit.fbx",
	L"First Aid Kit",
	Vec3(0.f, 0.f, 3000.f),
	Vec3(500.f, 500.f, 500.f),
	Vec3(0.f, 0.f, 0.f),
	{
		[](CGameObject* obj) {
			obj->AddComponent(new CCollider3D);
			obj->Collider3D()->SetName(L"Weapon");
			obj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
			obj->Collider3D()->SetIndependentScale(true);
			obj->Collider3D()->SetTrigger(true);
		},
		[](CGameObject* obj) {
			obj->AddComponent(new GunController);
			WeaponController* script = static_cast<WeaponController*>(obj->GetScripts()[0]);
			script->SetWeaponType(WEAPON_TYPE::PRIMARY);
		},
		[](CGameObject* obj) {
			obj->AddComponent(new ItemScript(ITEM_TYPE::FIRST_AID_KIT));
		}
	},
	6,
	false
	);

	SetupFBX(
	LevelForTest,
	L"FBX\\Props\\Heal\\Adrenaline Syringe.fbx",
	L"Adrenaline",
	Vec3(0.f, 0.f, 0.f), // 위치값이 원본 코드에선 따로 없음(필요시 Vec3(0.f, 0.f, 0.f) 등으로)
	Vec3(150.f, 150.f, 150.f),
	Vec3(0.f, 0.f, 0.f),
	{
		[](CGameObject* obj) {
			obj->AddComponent(new CCollider3D);
			obj->Collider3D()->SetName(L"Weapon");
			obj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
			obj->Collider3D()->SetIndependentScale(true);
			obj->Collider3D()->SetTrigger(true);
		},
		[](CGameObject* obj) {
			obj->AddComponent(new ItemScript(ITEM_TYPE::ADRENALINE_SYRINGE));
		}
	},
	6,
	false
	);

	auto* Bandage = SetupFBX(
	LevelForTest,
	L"FBX\\Props\\Heal\\Bandage.fbx",
	L"Bandage",
	Vec3(0.f, 0.f, 0.f), // 위치값이 원본 코드에선 따로 없음(필요시 Vec3(0.f, 0.f, 0.f) 등으로)
	Vec3(150.f, 150.f, 150.f),
	Vec3(0.f, 0.f, 0.f),
	{
		[](CGameObject* obj) {
			obj->AddComponent(new CCollider3D);
			obj->Collider3D()->SetName(L"Weapon");
			obj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
			obj->Collider3D()->SetIndependentScale(true);
			obj->Collider3D()->SetTrigger(true);
		},
		[](CGameObject* obj) {
			obj->AddComponent(new ItemScript(ITEM_TYPE::AMMO_5));
		}
	},
	6,
	false
);

	MakeClone(LevelForTest, Bandage, 6, false, 7);

	SetupFBX(
		LevelForTest,
		L"FBX\\Props\\Death Box\\box.fbx",
		L"DeathBox",
		Vec3(6590.f, -410.f, 5000.f),
		Vec3(700.f, 2000.f, 700.f),
		Vec3(0.f, 0.f, 0.f),
		{
			[](CGameObject* obj) {
				obj->AddComponentRecursive<CMeshCollider>();
				obj->Transform()->SetRelativeScaleMultiply(0.3f);
			}
		},
		1,
		true
	);

	// 적 테스트
	auto* TestTarget = SetupFBX(
		LevelForTest,
		L"FBX\\Testasset.fbx",
		L"TestTarget",
		Vec3(3000.f, 0.f, 1000.f),
		Vec3(5.f, 5.f, 5.f),
		Vec3(0.f, 0.f, 0.f),
		{
			[](CGameObject* obj) {
				obj->AddComponent(new CCollider3D);
				obj->Collider3D()->SetScale(Vec3(200.f, 200.f, 200.f));
			},
			[](CGameObject* obj) {
				obj->AddComponent(new TestCharacter);
			}
		},
		7,
		false
	);

	// 적 시야 테스트
	CGameObject* pVision = new CGameObject;
	pVision->SetName(L"TestVision");
	pVision->AddComponent(new CCollider3D);
	pVision->AddComponent(new CColliderRay);
	pVision->AddComponent(new EnemyVisionScript);
	pVision->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));

	pVision->Collider3D()->SetScale(Vec3(1000.f, 1000.f, 4000.f));
	pVision->Collider3D()->SetOffset(Vec3(0.f, 0.f, 2500.f));
	pVision->Collider3D()->SetIndependentScale(true);
	pVision->Collider3D()->SetTrigger(true);

	pVision->ColliderRay()->SetRayLength(1000);
	pVision->ColliderRay()->SetIndependentDir(true);
	pVision->ColliderRay()->SetTriggerTarget(false);

	TestTarget->AddChild(pVision);

	// Door
	SetupFBX(
		LevelForTest,
		L"FBX\\door.fbx",
		L"Door",
		Vec3(2000.f, -500.f, 1500.f),
		Vec3(1000.f, 1000.f, 1000.f),
		Vec3(0.f, 0.f, 0.f),
		{
			[](CGameObject* obj) {
				obj->AddComponent(new CCollider3D);
				obj->Collider3D()->SetScale(Vec3(800.f, 2000.f, 40.f));
				obj->Collider3D()->SetOffset(Vec3(400.f, 1000.f, 0.f));
				obj->Collider3D()->SetIndependentScale(true);
			},
			[](CGameObject* obj) {
				obj->AddComponent(new DoorScript);
			}
		},
		1,
		false
	);
	//
	// CGameObject* testPlayer = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Character\\GasMask.fbx")->Instantiate();
	// testPlayer->SetName(L"Test_Player");
	//
	// testPlayer->Transform()->SetRelativePos(Vec3(500.f, -380.f, 1000.f));
	// testPlayer->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));
	// testPlayer->Transform()->SetRelativeRotation(0.f, 90.f, 0.f);
	//
	// testPlayer->Animator3D()->SetAnimClip(pAnimationSet->GetAnimations());
	//
	// pLevel->AddObject(1, testPlayer, false);

	// Downtown Alley
	SetupFBX(
		LevelForTest,
		L"FBX\\Downtown_Alley_Scene.fbx",
		L"Downtown_Alley",
		Vec3(0.f, -1500.f, 0.f),
		Vec3(1.f, 1.f, 1.f), // 기본 스케일, 아래에서 Multiply로 조정
		Vec3(0.f, 0.f, 0.f),
		{
			[](CGameObject* obj) {
				obj->Transform()->SetRelativeScaleMultiply(4.f);
				obj->AddComponentRecursive<CMeshCollider>();
			}
		},
		1,
		true
	);
}

// TODO(KHJ): 이하의 내용 Factory Pattern 처리해서 추후 CLI 게임 개발 시 활용할 수 있도록 할 것

// Setting Level
void TestLevel::SetUpLayer(CLevel* PLevel)
{
	PLevel->GetLayer(0)->SetName(L"Background");
	PLevel->GetLayer(1)->SetName(L"Structure"); // 건물, 구조물 등
	PLevel->GetLayer(2)->SetName(L"Default");
	PLevel->GetLayer(3)->SetName(L"PlayerTPS");
	PLevel->GetLayer(4)->SetName(L"PlayerFPS");
	PLevel->GetLayer(5)->SetName(L"PlayerObject");
	PLevel->GetLayer(6)->SetName(L"Item");
	PLevel->GetLayer(7)->SetName(L"MonsterObject");
	PLevel->GetLayer(8)->SetName(L"UI");
	PLevel->GetLayer(9)->SetName(L"ObjectPool");
	PLevel->GetLayer(10)->SetName(L"BulletLayer");
}

// Collision Layer Setting
void TestLevel::SetUpCollision(CLevel* PLevel)
{
	CCollisionMgr::GetInst()->ToggleLayerCollision(0, 0);
	CCollisionMgr::GetInst()->ToggleLayerCollision(0, 1);
	CCollisionMgr::GetInst()->ToggleLayerCollision(0, 7);
	CCollisionMgr::GetInst()->ToggleLayerCollision(3, 0);
	CCollisionMgr::GetInst()->ToggleLayerCollision(3, 1);
	CCollisionMgr::GetInst()->ToggleLayerCollision(3, 6);
	CCollisionMgr::GetInst()->ToggleLayerCollision(3, 7);

	// 총알이 충돌될 레이어
	CCollisionMgr::GetInst()->ToggleLayerCollision(0, 10);
	CCollisionMgr::GetInst()->ToggleLayerCollision(1, 10);
	CCollisionMgr::GetInst()->ToggleLayerCollision(3, 10);
	CCollisionMgr::GetInst()->ToggleLayerCollision(4, 10);
	CCollisionMgr::GetInst()->ToggleLayerCollision(7, 10);
}

// Main Camera
void TestLevel::SetUpCamera(CLevel* PLevel)
{
	CGameObject* MainCamera = new CGameObject();
	MainCamera->SetName(L"MainCamera");
	MainCamera->AddComponent(new CameraController);

	MainCamera->AddComponent(new CCamera);
	MainCamera->Camera()->SetProjType(PERSPECTIVE);
	MainCamera->Camera()->SetPriority(0);
	MainCamera->Camera()->LayerCheckAll();
	MainCamera->Camera()->LayerCheck(4);

	MainCamera->AddComponent(new CColliderRay);
	MainCamera->ColliderRay()->SetRayDir(Vec3(0.f, 0.f, 1.f));
	MainCamera->ColliderRay()->SetRayLength(2000.f);
	MainCamera->ColliderRay()->SetIndependentDir(true);

	PLevel->AddObject(0, MainCamera, false);
}

// Point Light
void TestLevel::SetUpLight(CLevel* PLevel)
{
	CGameObject* PointLight = new CGameObject();
	PointLight->SetName(L"PointLight");

	PointLight->AddComponent(new CLight3D);
	PointLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	PointLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	PointLight->Light3D()->SetAmbient(Vec3(0.15f, 0.15f, 0.15f));
	PointLight->Light3D()->SetSpecularCoefficient(0.3f);
	PointLight->Light3D()->SetRadius(300.f);

	PointLight->Transform()->SetRelativePos(0.f, -450.f, 0.f);
	PointLight->Transform()->SetRelativeRotation(45.f, 45.f, 0.f);

	PLevel->AddObject(0, PointLight, false);
}

// Sky Box
void TestLevel::SetUpSkyBox(CLevel* PLevel)
{
	CGameObject* SkyBox = new CGameObject();
	SkyBox->SetName(L"SkyBox");
	Ptr<CTexture> SkyBoxTexture = CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\skybox\\Sky01.png");

	SkyBox->AddComponent(new CSkyBox);
	SkyBox->SkyBox()->SetMode(SPHERE);
	SkyBox->SkyBox()->SetSkyBoxTexture(SkyBoxTexture);

	// FrustumCheck 비활성화
	SkyBox->Transform()->SetFrustumCheck(false);

	PLevel->AddObject(0, SkyBox, false);
}

// Landscape
void TestLevel::SetUpLandscape(CLevel* PLevel)
{
	CGameObject* Landscape = new CGameObject();
	Landscape->SetName(L"LandScape");
	Landscape->AddComponent(new CLandScape);

	Landscape->Transform()->SetRelativePos(Vec3(0.f, -500.f, 0.f));
	Landscape->Transform()->SetRelativeScale(Vec3(500.f, 500.f, 500.f));

	Landscape->LandScape()->SetFace(32, 32);
	Landscape->LandScape()->CreateHeightMap(1024, 1024);
	Landscape->LandScape()->SetColorTexture(CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\LandScapeTexture\\LS_Color.dds"));
	Landscape->LandScape()->SetNormalTexture(CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\LandScapeTexture\\LS_Normal.dds"));

	PLevel->AddObject(0, Landscape, false);
}

// UI Preset
vector<CGameObject*> TestLevel::SetUpUI(CLevel* PLevel)
{
	// UI Camera
	CGameObject* UICamera = new CGameObject;
	UICamera->SetName(L"UICamera");
	UICamera->AddComponent(new CCamera);
	UICamera->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	UICamera->Camera()->SetPriority(1);
	UICamera->Camera()->SetFar(10.f);

	assert(PLevel->GetLayer(8)->GetName() == L"UI");
	UICamera->Camera()->LayerCheck(8);

	PLevel->AddObject(0, UICamera, false);

	// "Inventory CanvasUI"
	CGameObject* CanvasUI = new CGameObject;
	CanvasUI->SetName(L"Inventory_CanvasUI");
	CanvasUI->AddComponent(new CUI(UI_CANVAS));

	CanvasUI->AddComponent(new CUIRender);
	CanvasUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.3f));
	CanvasUI->UI()->SetPriority(0);
	CanvasUI->UI()->SetRectPos(0.f, 0.f);
	CanvasUI->UI()->SetRectSize(1280.f, 768.f);

	SetObjectActive(CanvasUI, false);

	PLevel->AddObject(8, CanvasUI, false);	// UI layer

	// DropUI
	CGameObject* UI = new CGameObject;
	UI->SetName(L"DropUI");
	UI->AddComponent(new CUI(UI_DROP));

	UI->AddComponent(new CUIRender);
	UI->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	UI->UI()->SetRectPos(400.f, 240.f);
	UI->UI()->SetRectSize(350.f, 120.f);
	UI->UI()->AddText(L"Drop", 0.f, 0.f, 16, FONT_RGBA(255, 20, 20, 255));
	CanvasUI->AddChild(UI);

	UI = UI->Clone();
	UI->UI()->SetRectPos(400.f, 100.f);
	CanvasUI->AddChild(UI);

	UI = UI->Clone();
	UI->UI()->SetRectPos(400.f, -40.f);
	CanvasUI->AddChild(UI);

	// 인벤토리
	CGameObject* Inventory = UI->Clone();
	Inventory->SetName(L"InventoryUI");
	Inventory->UI()->ClearText();
	Inventory->UI()->AddText(L"인벤토리", 5.f, 0.f, 16, FONT_RGBA(255, 20, 20, 255));
	Inventory->UI()->SetRectSize(160.f, 600.f);
	Inventory->UI()->SetRectPos(-200.f, 0.f);

	Inventory->AddComponent(new InventoryUI);

	CanvasUI->AddChild(Inventory);

	// 주변
	CGameObject* Vicinity = UI->Clone();
	Vicinity->SetName(L"VicinityUI");
	Vicinity->UI()->ClearText();
	Vicinity->UI()->AddText(L"주변", 5.f, 0.f, 16, FONT_RGBA(255, 20, 20, 255));
	Vicinity->UI()->SetRectSize(160.f, 600.f);
	Vicinity->UI()->SetRectPos(-440.f, 0.f);

	Vicinity->AddComponent(new VicinityUI);

	CanvasUI->AddChild(Vicinity);

	for (int i = 0; i < 20; ++i)
	{
		// DragUI
		CGameObject* DragUI = new CGameObject;
		DragUI->SetName(L"ItemUI");
		DragUI->AddComponent(new CUI(UI_DRAG | UI_RIGHT_CLICK));

		DragUI->AddComponent(new CUIRender);
		DragUI->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
		DragUI->UI()->SetRectSize(150.f, 40.f);
		DragUI->UI()->SetRectPos(0.f, 200.f - 43.f * i);
		SetObjectActive(DragUI, false);

		DragUI->AddComponent(new ItemUI);

		Vicinity->AddChild(DragUI);

		CGameObject* ChildUI = new CGameObject;
		ChildUI->SetName(L"ItemImageUI");
		ChildUI->AddComponent(new CUI);
		ChildUI->AddComponent(new CUIRender);
		ChildUI->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
		ChildUI->UI()->SetRectPos(-55.f, 0.f);
		ChildUI->UI()->SetRectSize(40.f, 40.f);

		DragUI->AddChild(ChildUI);
	}

	for (int i = 0; i < 20; ++i)
	{
		// DragUI
		CGameObject* DragUI = new CGameObject;
		DragUI->SetName(L"ItemUI");
		DragUI->AddComponent(new CUI(UI_DRAG | UI_RIGHT_CLICK));

		DragUI->AddComponent(new CUIRender);
		DragUI->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
		DragUI->UI()->SetRectSize(150.f, 40.f);
		DragUI->UI()->SetRectPos(0.f, 200.f - 43.f * i);
		SetObjectActive(DragUI, false);


		DragUI->AddComponent(new ItemUI);

		Inventory->AddChild(DragUI);

		CGameObject* ChildUI = new CGameObject;
		ChildUI->SetName(L"ItemImageUI");
		ChildUI->AddComponent(new CUI);
		ChildUI->AddComponent(new CUIRender);
		ChildUI->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
		ChildUI->UI()->SetRectPos(-55.f, 0.f);
		ChildUI->UI()->SetRectSize(40.f, 40.f);

		DragUI->AddChild(ChildUI);
	}

	// "Cardinal Direction Canvas UI"
	CanvasUI = new CGameObject;
	CanvasUI->SetName(L"Cardinal_CanvasUI");
	CanvasUI->AddComponent(new CUI(UI_CANVAS));
	CanvasUI->UI()->SetRectPos(Vec2(0.f, 320.f));
	CanvasUI->UI()->SetRectSize(Vec2(560.f, 55.f));

	CanvasUI->AddComponent(new CUIRender);
	CanvasUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.1f));
	CanvasUI->UI()->SetPriority(1);

	PLevel->AddObject(8, CanvasUI, false);

	CGameObject* pImageUI = new CGameObject;
	pImageUI->SetName(L"Cardinal_ImageUI");
	pImageUI->AddComponent(new CUI);
	pImageUI->UI()->SetImage(CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\UI\\Cardinal.png", L"Texture\\UI\\Cardinal.png"));
	pImageUI->UI()->SetRectSize(Vec2(1140.f, 48.f));
	pImageUI->UI()->SetRectPos(Vec2(0.f, -7.f));

	pImageUI->AddComponent(new CUIRender);
	pImageUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UICardinalMtrl"), 0);

	CanvasUI->AddChild(pImageUI);

	pImageUI = new CGameObject;
	pImageUI->SetName(L"Cardinal_ArrowUI");
	pImageUI->AddComponent(new CUI);
	pImageUI->UI()->SetImage(CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\UI\\Cardinal_Arrow.png", L"Texture\\UI\\Cardinal_Arrow.png"));
	pImageUI->UI()->SetRectSize(Vec2(15.f, 15.f));
	pImageUI->UI()->SetRectPos(Vec2(0.f, 20.f));

	pImageUI->AddComponent(new CUIRender);

	CanvasUI->AddChild(pImageUI);

	// "Main Canvas UI" : 화면 전체 가리는 투명 ui
	CanvasUI = new CGameObject;
	CanvasUI->SetName(L"Main_CanvasUI");
	CanvasUI->AddComponent(new CUI(UI_CANVAS));
	CanvasUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));
	CanvasUI->UI()->SetPriority(0);
	CanvasUI->UI()->SetRectPos(0.f, 0.f);
	CanvasUI->UI()->SetRectSize(1280.f, 768.f);

	CanvasUI->AddComponent(new CUIRender);

	PLevel->AddObject(8, CanvasUI, false);

	// HP UI
	CGameObject* childUI = new CGameObject;
	childUI->SetName(L"HP_UI");
	childUI->AddComponent(new CUI);
	childUI->UI()->SetRectPos(Vec2(0.f, -350.f));
	childUI->UI()->SetRectSize(Vec2(280.f, 18.f));

	childUI->AddComponent(new CUIRender);
	childUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.4f));

	childUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIHPMtrl"), 0);

	CanvasUI->AddChild(childUI);

	// ItemUse UI
	childUI = new CGameObject;
	childUI->SetName(L"ItemUse_UI");
	childUI->AddComponent(new CUI);
	childUI->UI()->SetRectPos(Vec2(0.f, 0.f));
	childUI->UI()->SetRectSize(Vec2(60.f, 60.f));

	childUI->AddComponent(new CUIRender);
	childUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.6f));

	childUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIItemUseMtrl"), 0);
	childUI->UI()->AddText(L"", 17.f, 16.f, 20, FONT_RGBA(255, 255, 255, 255));
	SetObjectActive(childUI, false);

	CanvasUI->AddChild(childUI);

	// Interaction UI
	CGameObject* interactionUI = new CGameObject;
	interactionUI->SetName(L"Interaction_UI");
	interactionUI->AddComponent(new CUI);
	interactionUI->UI()->SetRectPos(Vec2(100.f, -50.f));
	interactionUI->UI()->SetRectSize(Vec2(80.f, 30.f));

	interactionUI->AddComponent(new CUIRender);
	interactionUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.6f));

	interactionUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
	interactionUI->UI()->AddText(L"취소", 40.f, 4.f, 16, FONT_RGBA(255, 255, 255, 255));
	SetObjectActive(interactionUI, false);

	CanvasUI->AddChild(interactionUI);

	// Interaction Key UI
	childUI = new CGameObject;
	childUI->SetName(L"InteractionKey_UI");
	childUI->AddComponent(new CUI);
	childUI->UI()->SetRectPos(Vec2(-25.f, 0.f));
	childUI->UI()->SetRectSize(Vec2(30.f, 30.f));

	childUI->AddComponent(new CUIRender);
	childUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.6f));

	childUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
	childUI->UI()->AddText(L"F", 11.f, 4.f, 16, FONT_RGBA(255, 255, 255, 255));

	interactionUI->AddChild(childUI);

	// Rounds UI
	CGameObject* RoundsUI = new CGameObject;
	RoundsUI->SetName(L"RoundsView_UI");
	RoundsUI->AddComponent(new CUI);
	RoundsUI->UI()->SetRectPos(Vec2(0.f, -320.f));
	RoundsUI->UI()->SetRectSize(Vec2(100.f, 40.f));

	RoundsUI->AddComponent(new CUIRender);
	RoundsUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));

	RoundsUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
	RoundsUI->AddComponent(new RoundsUIScript);

	CanvasUI->AddChild(RoundsUI);

	// Kill Info
	CGameObject* KillinfoUI = new CGameObject;
	KillinfoUI->SetName(L"Killinfo_UI");
	KillinfoUI->AddComponent(new CUI);
	KillinfoUI->UI()->SetRectPos(Vec2(0.f, -200.f));
	KillinfoUI->UI()->SetRectSize(Vec2(500.f, 40.f));

	KillinfoUI->AddComponent(new CUIRender);
	KillinfoUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));

	KillinfoUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
	KillinfoUI->AddComponent(new KillinfoUIScript);

	CanvasUI->AddChild(KillinfoUI);

	// 크로스헤어 UI
	childUI = new CGameObject;
	childUI->SetName(L"CrosshairUI");
	childUI->AddComponent(new CUI);
	childUI->UI()->SetRectPos(Vec2(0.f, 0.f));
	childUI->UI()->SetRectSize(Vec2(70.f, 70.f));
	childUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));
	childUI->UI()->SetImage(CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\UI\\CrossHair_1.png"));

	childUI->AddComponent(new CUIRender);
	childUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UICrosshairMtrl"), 0);
	childUI->UIRender()->GetMaterial(0)->SetScalarParam(INT_0, 1);
	childUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, 0.08f);
	childUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, 0.03f);
	childUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_2, 0.2f);
	childUI->UIRender()->GetMaterial(0)->SetScalarParam(VEC4_1, Vec4(0.0f, 1.0f, 0.0f, 1.0f));

	CanvasUI->AddChild(childUI);

	return {Vicinity, Inventory, interactionUI};
}

// TODO(KHJ): FBX 기반 Common Load Method 구축할 것

void TestLevel::SetUpPlayer(CLevel* PLevel, vector<CGameObject*>& PUIInfo)
{
	// Load Animation & MeshData From FBX
	Ptr<CMeshData> PlayerMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Character\\James.fbx");
	Ptr<CMeshData> pAnimationSet = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Character\\James_Anim.fbx");

	CGameObject* Player = PlayerMeshData->Instantiate();
	Player->SetName(L"Player");
	Player->AddComponent(new CCollider3D);
	Player->AddComponent(new CColliderRay);
	Player->AddComponent(new PlayerCharacter);
	Player->AddComponent(new InventoryController);

	Player->Transform()->SetRelativePos(Vec3(6500.f, 5000.f, 4500.f));
	Player->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));
	Player->Transform()->SetRelativeRotation(0.f, 90.f, 0.f);

	Player->ColliderRay()->SetRayDir(Vec3(0.f, 0.f, -1.f));
	Player->ColliderRay()->SetOffset(Vec3(0.f, 1500.f, 0.f));
	Player->ColliderRay()->SetRayLength(5000.f);
	Player->ColliderRay()->SetTriggerTarget(true);

	Player->Animator3D()->SetAnimClip(pAnimationSet->GetAnimations());

	Player->Collider3D()->SetScale(Vec3(550.f, 1600.f, 385.f));
	Player->Collider3D()->SetOffset(Vec3(35.f, 760.f, 0.f));
	Player->Collider3D()->SetIndependentScale(true);


	InventoryController* InventoryScript = static_cast<InventoryController*>(GetScriptWithType(
		Player, SCRIPT_TYPE::INVENTORYSCRIPT));

	// 주변 및 인벤토리 UI를 등록함
	InventoryScript->SetVicinityUI(PUIInfo[0]);
	InventoryScript->SetInventoryUI(PUIInfo[1]);
	InventoryScript->SetPlayer(Player);

	// Interaction Object
	CGameObject* pInteractionHandler = new CGameObject;
	pInteractionHandler->SetName(L"Interaction Handler");
	pInteractionHandler->AddComponent(new CCollider3D);

	pInteractionHandler->Collider3D()->SetScale(Vec3(2000.f, 2000.f, 2000.f));
	pInteractionHandler->Collider3D()->SetIndependentScale(true);
	pInteractionHandler->Collider3D()->SetTrigger(true);
	pInteractionHandler->Collider3D()->SetOffset(Vec3(0.f, 1000.f, 0.f));

	auto pHandlerScript = new InteractionHandler;

	pHandlerScript->SetPlayer(Player);
	pHandlerScript->SetInteractionUI(PUIInfo[2]);

	pInteractionHandler->AddComponent(pHandlerScript);

	Player->AddChild(pInteractionHandler);

	// Player Head Collider
	CGameObject* pHeadColl = new CGameObject;
	pHeadColl->SetName(L"Player Head");
	pHeadColl->AddComponent(new CCollider3D);

	pHeadColl->Transform()->SetRelativePos(Vec3(0.f, 170.f, 0.f));

	pHeadColl->Collider3D()->SetScale(Vec3(200.f, 200.f, 200.f));
	pHeadColl->Collider3D()->SetIndependentScale(true);
	pHeadColl->Collider3D()->SetTrigger(false);

	Player->AddChild(pHeadColl);

	PLevel->AddObject(3, Player, true);
}

void TestLevel::SetUpWeapon(CLevel* PLevel)
{
	Ptr<CMeshData> WeaponFBX = CAssetMgr::GetInst()->LoadFBX(L"FBX\\ak47_test.fbx");

	CGameObject* Weapon = WeaponFBX->Instantiate();
	Weapon->SetName(L"AKM");
	Weapon->AddComponent(new CColliderRay);
	Weapon->AddComponent(new CCollider3D);
	Weapon->AddComponent(new GunController);

	WeaponController* pScript = static_cast<WeaponController*>(Weapon->GetScripts()[0]);
	pScript->SetWeaponType(WEAPON_TYPE::PRIMARY);

	Weapon->Transform()->SetRelativePos(Vec3(90.f, 0.f, 30.f));
	Weapon->Transform()->SetRelativeScale(Vec3(900.f, 900.f, 900.f));
	Weapon->Transform()->SetRelativeRotation(0.f, -2.f, -4.3f);
	Weapon->Transform()->SetIndependentScale(true);

	Weapon->Collider3D()->SetName(L"Weapon");
	Weapon->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
	Weapon->Collider3D()->SetIndependentScale(true);
	Weapon->Collider3D()->SetTrigger(true);

	Weapon->ColliderRay()->SetRayPos(Vec3(-30.f, 100.f, 0.f));
	Weapon->ColliderRay()->SetRayDir(Vec3(1.f, 0.f, 0.f));
	Weapon->ColliderRay()->SetOffset(Vec3(556.f, 72.f, 0.f));

	Weapon->AddComponent(new ItemScript(ITEM_TYPE::AKM));

	PLevel->AddObject(6, Weapon, false);
}

void TestLevel::SetUpGrenade(CLevel* PLevel)
{
	Ptr<CMeshData> GrenadeFBX = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Throwable\\GRENADE.fbx");

	CGameObject* Grenade = GrenadeFBX->Instantiate();
	Grenade->SetName(L"Grenade");
	Grenade->AddComponent(new CCollider3D);
	Grenade->AddComponent(new ThrowableController);

	WeaponController* pScript1 = static_cast<WeaponController*>(Grenade->GetScripts()[0]);
	pScript1->SetWeaponType(WEAPON_TYPE::THROWABLE);

	Grenade->Collider3D()->SetName(L"Weapon");
	Grenade->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
	Grenade->Collider3D()->SetIndependentScale(true);
	Grenade->Collider3D()->SetTrigger(true);

	Grenade->Transform()->SetRelativePos(0.f, 0.f, 5000.f);
	Grenade->Transform()->SetRelativeScale(Vec3(15.f, 15.f, 15.f));
	Grenade->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
	Grenade->Transform()->SetIndependentScale(true);

	Grenade->AddComponent(new ItemScript(ITEM_TYPE::GRENADE));

	// TODO(KHJ): Clone 용이성 고려할 것
	PLevel->AddObject(6, Grenade, false);
	PLevel->AddObject(6, Grenade->Clone(), false);
}

CGameObject* TestLevel::SetupFBX(
	CLevel* PLevel,
	const wstring& PFilePath,
	const wstring& PName,
	const Vec3& PPos,
	const Vec3& PScale,
	const Vec3& PRotation,
	const vector<function<void(CGameObject*)>>& ComponentSetups,
	int PLayer,
	bool PMoveWithChild
)
{
	// FBX 로드 및 인스턴스화
	Ptr<CMeshData> MeshData = CAssetMgr::GetInst()->LoadFBX(PFilePath.c_str());
	CGameObject* Object = MeshData->Instantiate();
	Object->SetName(PName.c_str());

	// Transform 세팅
	Object->Transform()->SetRelativePos(PPos);
	Object->Transform()->SetRelativeScale(PScale);
	Object->Transform()->SetRelativeRotation(PRotation);

	// 컴포넌트 추가 및 세팅
	for (const auto& Setup : ComponentSetups)
	{
		Setup(Object);
	}

	// 레벨에 오브젝트 추가
	PLevel->AddObject(PLayer, Object, PMoveWithChild);

	return Object;
}

void TestLevel::MakeClone(CLevel* PLevel, CGameObject* POrigin, int PLayer, bool PMoveWithChild, int PCopyNum)
{
	for (int i = 0; i <= PCopyNum; i++)
	{
		PLevel->AddObject(PLayer, POrigin->Clone(), PMoveWithChild);
	}
}
