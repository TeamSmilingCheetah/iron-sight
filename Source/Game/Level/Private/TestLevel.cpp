#include "pch.h"
#include "Game/Level/Public/TestLevel.h"

#include "Engine/API/Public/EngineFacade.h"

#include "Game/Factory/Public/GameFactory.h"
#include "Game/Gameplay/states.h"
#include "Game/Gameplay/scripts.h"
#include "Game/System/Public/CGameMgr.h"
#include "Game/Gameplay/Event/Public/LevelRestart.h"

// TODO(KHJ): 이하 헤더 배제 시도
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"
#include "Engine/Runtime/Public/Component/Physics/RayCollider.h"
#include "Engine/Runtime/Public/Component/Physics/MeshCollider.h"
#include "Engine/Runtime/Public/Component/Physics/SphereCollider.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"

class CLevel;

// XXX(KHJ): 스마트 포인터로 관리 시도? 일단 받아오는 과정은 스마트 포인터로 처리
void TestLevel::CreateTestLevel()
{
	// Level Creation By Game Factory & Setting Current Level
	unique_ptr<CLevel> LevelForTest = GameFactory::CreateLevel();

	// Release Unique Ptr & Use Raw Ptr
	CLevel* LevelRawPtr = LevelForTest.get();
	LevelForTest.release();

	ChangeLevel(LevelRawPtr, LEVEL_STATE::STOP);

	// Level Base Setting
	GameFactory::LoadDefaultLayer(LevelRawPtr);
	GameFactory::LoadDefaultCollisionSetting();
	GameFactory::LoadMainCamera(LevelRawPtr);
	GameFactory::LoadDefaultLight(LevelRawPtr);
	GameFactory::LoadDefaultSkyBox(LevelRawPtr);
	//GameFactory::LoadDefaultLandscape(LevelRawPtr);

	// Initialize Item Parts
	auto UIInfo = SetUpUI(LevelRawPtr);
	ItemMgr::GetInst()->Init();

	SetUpPlayer(LevelRawPtr, UIInfo);
	SetUpEvent(LevelRawPtr);

	// Weapon
	GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\ak47_test.fbx",
		L"AKM",
		Vec3(2770.f, 0.f, 1640.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(700.f, 700.f, 700.f),
		{
			[](CGameObject* obj)
			{
				Engine::Common::AddComponentToObject<FRayCollider>(obj);
				Engine::Common::AddComponentToObject<FBoxCollider>(obj);
				obj->BoxCollider()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->BoxCollider()->SetIndependentScale(true);
				obj->BoxCollider()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::BoxCollider);

			},
			[](CGameObject* obj)
			{
				Engine::Common::AddScriptToObject(obj, SCRIPT_TYPE::GUNSCRIPT);
				// TODO(KHJ): Wrapping
				WeaponController* script = static_cast<WeaponController*>(obj->GetScripts()[0]);
				script->SetWeaponType(WEAPON_TYPE::PRIMARY);
			},
			[](CGameObject* obj)
			{
				// TODO(KHJ): Wrapping
				obj->AddComponent(new ItemScript(ITEM_TYPE::AKM));
			}
		},
		6,
		false
	);

	// Grenade
	auto* Grenade = GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\Props\\Throwable\\GRENADE.fbx",
		L"Grenade",
		Vec3(0.f, 0.f, 0.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(10.f, 10.f, 10.f),
		{
			[](CGameObject* obj)
			{
				Engine::Common::AddComponentToObject<FBoxCollider>(obj);
				// TODO(KHJ): Wrapping
				obj->BoxCollider()->SetName(L"Weapon");
				obj->BoxCollider()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->BoxCollider()->SetIndependentScale(true);
				obj->BoxCollider()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::BoxCollider);
			},
			[](CGameObject* obj)
			{
				Engine::Common::AddScriptToObject(obj, SCRIPT_TYPE::THROWABLESCRIPT);
				// TODO(KHJ): Wrapping
				WeaponController* script = static_cast<WeaponController*>(obj->GetScripts()[0]);
				script->SetWeaponType(WEAPON_TYPE::THROWABLE);
			},
			[](CGameObject* obj)
			{
				// TODO(KHJ): Wrapping
				obj->AddComponent(new ItemScript(ITEM_TYPE::GRENADE));
			}
		},
		6,
		false
	);

	GameFactory::MakeCloneObject(LevelRawPtr, Grenade, false, 1);

	// Smoke
	GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\Props\\Throwable\\M18_Smoke.fbx",
		L"Smoke Grenade",
		Vec3(0.f, 0.f, 0.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(10.f, 10.f, 10.f),
		{
			[](CGameObject* obj)
			{
				Engine::Common::AddComponentToObject<FBoxCollider>(obj);
				// TODO(KHJ): Wrapping
				obj->BoxCollider()->SetName(L"Weapon");
				obj->BoxCollider()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->BoxCollider()->SetIndependentScale(true);
				obj->BoxCollider()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::BoxCollider);
			},
			[](CGameObject* obj)
			{
				Engine::Common::AddScriptToObject(obj, SCRIPT_TYPE::THROWABLESCRIPT);
				// TODO(KHJ): Wrapping
				WeaponController* script = static_cast<WeaponController*>(obj->GetScripts()[0]);
				script->SetWeaponType(WEAPON_TYPE::THROWABLE);
			},
			[](CGameObject* obj)
			{
				// TODO(KHJ): Wrapping
				obj->AddComponent(new ItemScript(ITEM_TYPE::SMOKEBOMB));
			}
		},
		6,
		false
	);

	// Test Primary Weapon
	GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\Props\\Heal\\Energy Drink.fbx",
		L"Energy Drink",
		Vec3(0.f, 0.f, 1000.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(500.f, 500.f, 500.f),
		{
			[](CGameObject* obj)
			{
				Engine::Common::AddComponentToObject<FBoxCollider>(obj);
				// TODO(KHJ): Wrapping
				obj->BoxCollider()->SetName(L"Weapon");
				obj->BoxCollider()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->BoxCollider()->SetIndependentScale(true);
				obj->BoxCollider()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::BoxCollider);
			},
			[](CGameObject* obj)
			{
				Engine::Common::AddScriptToObject(obj, SCRIPT_TYPE::GUNSCRIPT);
				// TODO(KHJ): Wrapping
				WeaponController* script = static_cast<WeaponController*>(obj->GetScripts()[0]);
				script->SetWeaponType(WEAPON_TYPE::SECONDARY);
			},
			[](CGameObject* obj)
			{
				// TODO(KHJ): Wrapping
				obj->AddComponent(new ItemScript(ITEM_TYPE::ENERGY_DRINK));
			}
		},
		6,
		false
	);

	// Secondary Weapon
	GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\Props\\Heal\\First Aid Kit.fbx",
		L"First Aid Kit",
		Vec3(0.f, 0.f, 3000.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(500.f, 500.f, 500.f),
		{
			[](CGameObject* obj)
			{
				Engine::Common::AddComponentToObject<FBoxCollider>(obj);
				// TODO(KHJ): Wrapping
				obj->BoxCollider()->SetName(L"Weapon");
				obj->BoxCollider()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->BoxCollider()->SetIndependentScale(true);
				obj->BoxCollider()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::BoxCollider);
			},
			[](CGameObject* obj)
			{
				obj->AddComponent(new GunController);
				// TODO(KHJ): Wrapping
				WeaponController* script = static_cast<WeaponController*>(obj->GetScripts()[0]);
				script->SetWeaponType(WEAPON_TYPE::PRIMARY);
			},
			[](CGameObject* obj)
			{
				// TODO(KHJ): Wrapping
				obj->AddComponent(new ItemScript(ITEM_TYPE::FIRST_AID_KIT));
			}
		},
		6,
		false
	);

	// Adrenaline
	GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\Props\\Heal\\Adrenaline Syringe.fbx",
		L"Adrenaline",
		Vec3(0.f, 0.f, 0.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(150.f, 150.f, 150.f),
		{
			[](CGameObject* obj)
			{
				Engine::Common::AddComponentToObject<FBoxCollider>(obj);
				// TODO(KHJ): Wrapping
				obj->BoxCollider()->SetName(L"Weapon");
				obj->BoxCollider()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->BoxCollider()->SetIndependentScale(true);
				obj->BoxCollider()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::BoxCollider);
			},
			[](CGameObject* obj)
			{
				// TODO(KHJ): Wrapping
				obj->AddComponent(new ItemScript(ITEM_TYPE::ADRENALINE_SYRINGE));
			}
		},
		6,
		false
	);

	// Bandage
	auto* Bandage = GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\Props\\Heal\\Bandage.fbx",
		L"Bandage",
		Vec3(0.f, 0.f, 0.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(150.f, 150.f, 150.f),
		{
			[](CGameObject* obj)
			{
				Engine::Common::AddComponentToObject<FBoxCollider>(obj);
				// TODO(KHJ): Wrapping
				obj->BoxCollider()->SetName(L"Weapon");
				obj->BoxCollider()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->BoxCollider()->SetIndependentScale(true);
				obj->BoxCollider()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::BoxCollider);
			},
			[](CGameObject* obj)
			{
				// TODO(KHJ): Wrapping
				obj->AddComponent(new ItemScript(ITEM_TYPE::AMMO_5));
			}
		},
		6,
		false
	);

	GameFactory::MakeCloneObject(LevelRawPtr, Bandage, false, 7);

	// Box
	GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\Props\\Death Box\\box.fbx",
		L"DeathBox",
		Vec3(6590.f, -410.f, 5000.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(700.f, 2000.f, 700.f),
		{
			[](CGameObject* obj)
			{
				obj->AddComponentRecursive<FMeshCollider>();
				obj->Transform()->SetRelativeScaleMultiply(0.3f);
			}
		},
		1,
		true
	);

	// Enemy Test
	auto* TestTarget = GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\Testasset.fbx",
		L"TestTarget",
		Vec3(20000.f, 0.f, 1000.f),
		Vec3(0.f, 270.f, 0.f),
		Vec3(5.f, 5.f, 5.f),
		{
			[](CGameObject* obj)
			{
				Engine::Common::AddComponentToObject<FBoxCollider>(obj);
				// TODO(KHJ): Wrapping
				Engine::Collider::SetColliderProperties(obj, {1000, 500, 1000}, {0, 50, 0}, true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::BoxCollider);
			},
			[](CGameObject* obj)
			{
				// TODO(KHJ): Wrapping
				obj->AddComponent(new TestCharacter);
			}
		},
		7,
		false
	);

	// 적 시야 테스트
	CGameObject* pVision = new CGameObject;
	pVision->SetName(L"TestVision");
	pVision->AddComponent(new EnemyVisionScript);
	pVision->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));

	//pVision->AddComponent(new FBoxCollider);
	//Engine::Collider::SetColliderDynamic(pVision, EColliderType::BoxCollider);
	//pVision->BoxCollider()->SetScale(Vec3(1000.f, 1000.f, 4000.f));
	//pVision->BoxCollider()->SetOffset(Vec3(0.f, 0.f, 2500.f));
	//pVision->BoxCollider()->SetIndependentScale(true);
	//pVision->BoxCollider()->SetTrigger(true);

	pVision->AddComponent(new FRayCollider);
	pVision->RayCollider()->SetLength(1000);
	pVision->RayCollider()->SetDirection({-1, 0, 0});
	pVision->RayCollider()->SetIndependentDir(true);
	pVision->RayCollider()->SetTriggerTarget(false);

	TestTarget->AddChild(pVision);

	// Door
	GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\door.fbx",
		L"Door",
		Vec3(18000.f, -800.f, 7100.f),
		Vec3(0.f, 350.f, 0.f),
		Vec3(800.f, 800.f, 800.f),
		{
			[](CGameObject* obj)
			{
				obj->AddComponent(new FBoxCollider);
				// TODO(KHJ): Wrapping
				obj->BoxCollider()->SetScale(Vec3(650.f, 1600.f, 40.f));
				obj->BoxCollider()->SetOffset(Vec3(310.f, 800.f, 0.f));
				obj->BoxCollider()->SetIndependentScale(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::BoxCollider);
			},
			[](CGameObject* obj)
			{
				// TODO(KHJ): Wrapping
				obj->AddComponent(new DoorScript);
			}
		},
		1,
		false
	);

	// CGameObject* testPlayer = FAssetMgr::GetInst()->LoadFBX(L"FBX\\Character\\GasMask.fbx")->Instantiate();
	// testPlayer->SetName(L"Test_Player");
	//
	// testPlayer->Transform()->SetRelativePos(Vec3(500.f, -380.f, 1000.f));
	// testPlayer->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));
	// testPlayer->Transform()->SetRelativeRotation(0.f, 90.f, 0.f);
	//
	// testPlayer->Animator3D()->SetAnimClip(pAnimationSet->GetAnimations());
	//
	// pLevel->AddObject(1, testPlayer, false);

	// Temp Landscape
	 GameFactory::MakeFBXObject(
	 	LevelRawPtr,
	 	L"FBX\\Props\\Death Box\\box.fbx",
	 	L"TempLandscape",
	 	Vec3(0.f, -800.f, 0.f),
	 	Vec3(0.f, 0.f, 0.f),
	 	Vec3(20000.f, 200.f, 20000.f),
	 	{
	 		[](CGameObject* obj)
	 		{
	 			obj->AddComponentRecursive<FMeshCollider>();
	 			obj->MeshCollider()->SetMeshType(EMeshColliderType::Original);
	 			obj->Transform()->SetFrustumCheck(false);
	 		}
	 	},
	 	1,
	 	true
	 );

	//GameFactory::MakeFBXObject(
	//	LevelRawPtr,
	//	L"FBX\\Downtown_Alley_Scene.fbx",
	//	L"Downtown_Alley",
	//	Vec3(0.f, -1500.f, 0.f),
	//	Vec3(0.f, 0.f, 0.f),
	//	Vec3(1.f, 1.f, 1.f), // 기본 스케일, 아래에서 Multiply로 조정
	//	{
	//		[](CGameObject* obj)
	//		{
	//			obj->Transform()->SetRelativeScaleMultiply(4.f);
	//			obj->AddComponentRecursive<FMeshCollider>();
	//		}
	//	},
	//	1,
	//	true
	//);


	 // PostProcess
	 CGameObject* pCameraPost = new CGameObject;
	 pCameraPost->SetName(L"CameraPost");
	 pCameraPost->Transform()->SetFrustumCheck(false);
	 pCameraPost->AddComponent(new CMeshRender);
	 pCameraPost->MeshRender()->SetMesh(FAssetManager::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	 pCameraPost->MeshRender()->SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"CameraPostMtrl"), 0);
	 pCameraPost->AddComponent(new CameraEffect);


	 LevelRawPtr->AddObject(20, pCameraPost, false);

}

// TODO(KHJ): 이하의 내용 Factory Pattern 처리해서 추후 CLI 게임 개발 시 활용할 수 있도록 할 것

// UI Preset
vector<CGameObject*> TestLevel::SetUpUI(CLevel* PLevel)
{
	// UI Camera
	CGameObject* UICamera = new CGameObject;
	UICamera->SetName(L"UICamera");
	UICamera->AddComponent(new CCamera);
	UICamera->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	UICamera->Camera()->SetPriority(2);
	UICamera->Camera()->SetFar(10.f);

	assert(PLevel->GetLayer(8)->GetName() == L"UI");
	UICamera->Camera()->LayerOn(8);
	UICamera->Camera()->LayerOn(20);
	PLevel->AddObject(0, UICamera, false);

	// "Inventory CanvasUI"
	CGameObject* InventoryCanvasUI = new CGameObject;
	InventoryCanvasUI->SetName(L"Inventory_CanvasUI");
	InventoryCanvasUI->AddComponent(new CUI(UI_CANVAS));

	InventoryCanvasUI->AddComponent(new CUIRender);
	InventoryCanvasUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.3f));
	InventoryCanvasUI->UI()->SetPriority(2);
	InventoryCanvasUI->UI()->SetRectPos(0.f, 0.f);
	InventoryCanvasUI->UI()->SetRectSize(1280.f, 768.f);

	SetObjectActive(InventoryCanvasUI, false);

	PLevel->AddObject(8, InventoryCanvasUI, false); // UI layer

	// DropUI
	CGameObject* UI = new CGameObject;
	UI->SetName(L"DropUI");
	UI->AddComponent(new CUI(UI_DROP));

	UI->AddComponent(new CUIRender);
	UI->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	UI->UI()->SetRectPos(400.f, 240.f);
	UI->UI()->SetRectSize(350.f, 120.f);
	UI->UI()->AddText(L"Drop", 0.f, 0.f, 16, FONT_RGBA(255, 20, 20, 255));
	InventoryCanvasUI->AddChild(UI);

	UI = UI->Clone();
	UI->UI()->SetRectPos(400.f, 100.f);
	InventoryCanvasUI->AddChild(UI);

	UI = UI->Clone();
	UI->UI()->SetRectPos(400.f, -40.f);
	InventoryCanvasUI->AddChild(UI);

	// 인벤토리
	CGameObject* Inventory = UI->Clone();
	Inventory->SetName(L"InventoryUI");
	Inventory->UI()->ClearText();
	Inventory->UI()->AddText(L"인벤토리", 5.f, 0.f, 16, FONT_RGBA(255, 20, 20, 255));
	Inventory->UI()->SetRectSize(160.f, 600.f);
	Inventory->UI()->SetRectPos(-200.f, 0.f);

	Inventory->AddComponent(new InventoryUI);

	InventoryCanvasUI->AddChild(Inventory);

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

	// 주변
	CGameObject* Vicinity = UI->Clone();
	Vicinity->SetName(L"VicinityUI");
	Vicinity->UI()->ClearText();
	Vicinity->UI()->AddText(L"주변", 5.f, 0.f, 16, FONT_RGBA(255, 20, 20, 255));
	Vicinity->UI()->SetRectSize(160.f, 600.f);
	Vicinity->UI()->SetRectPos(-440.f, 0.f);

	Vicinity->AddComponent(new VicinityUI);

	InventoryCanvasUI->AddChild(Vicinity);

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

	// "PauseMenu CanvasUI"
	CGameObject* PauseMenuCanvasUI = new CGameObject;
	PauseMenuCanvasUI->SetName(L"Pause_CanvasUI");
	PauseMenuCanvasUI->AddComponent(new CUI(UI_CANVAS));

	PauseMenuCanvasUI->AddComponent(new CUIRender);
	PauseMenuCanvasUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.5f));
	PauseMenuCanvasUI->UI()->SetPriority(0);
	PauseMenuCanvasUI->UI()->SetRectPos(0.f, 0.f);
	PauseMenuCanvasUI->UI()->SetRectSize(1280.f, 768.f);

	SetObjectActive(PauseMenuCanvasUI, false);

	PLevel->AddObject(8, PauseMenuCanvasUI, false); // UI layer

	// Continue
	CGameObject* Continue = new CGameObject;
	Continue->AddComponent(new CUI(UI_HOVER | UI_CLICK));

	Continue->AddComponent(new CUIRender);
	Continue->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	Continue->SetName(L"ContinueUI");
	Continue->UI()->ClearText();
	Continue->UI()->AddText(L"CONTINUE", 90.f, 0.f, 32, FONT_RGBA(0, 0, 0, 188));
	Continue->UI()->SetRectSize(320.f, 50.f);
	Continue->UI()->SetRectPos(0.f, 140.f);

	Continue->AddComponent(new PauseUIScript([]() { CGameMgr::GetInst()->ResumeGame(); }));

	PauseMenuCanvasUI->AddChild(Continue);

	// Restart
	CGameObject* Restart = new CGameObject;
	Restart->AddComponent(new CUI(UI_HOVER | UI_CLICK));

	Restart->AddComponent(new CUIRender);
	Restart->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	Restart->SetName(L"RestartUI");
	Restart->UI()->ClearText();
	Restart->UI()->AddText(L"RESTART", 100.f, 0.f, 32, FONT_RGBA(0, 0, 0, 188));
	Restart->UI()->SetRectSize(320.f, 50.f);
	Restart->UI()->SetRectPos(0.f, 60.f);

	Restart->AddComponent(new PauseUIScript([]() { CGameMgr::GetInst()->RestartGame(); }));

	PauseMenuCanvasUI->AddChild(Restart);

	// Option
	CGameObject* Option = new CGameObject;
	Option->AddComponent(new CUI(UI_HOVER | UI_CLICK));

	Option->AddComponent(new CUIRender);
	Option->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	Option->SetName(L"OptionUI");
	Option->UI()->ClearText();
	Option->UI()->AddText(L"OPTION", 105.f, 0.f, 32, FONT_RGBA(0, 0, 0, 188));
	Option->UI()->SetRectSize(320.f, 50.f);
	Option->UI()->SetRectPos(0.f, -20.f);

	Option->AddComponent(new PauseUIScript([]() { CGameMgr::GetInst()->OpenOption(); }));

	PauseMenuCanvasUI->AddChild(Option);

	// Exit
	CGameObject* Exit = new CGameObject;
	Exit->AddComponent(new CUI(UI_HOVER | UI_CLICK));

	Exit->AddComponent(new CUIRender);
	Exit->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	Exit->SetName(L"ExitUI");
	Exit->UI()->ClearText();
	Exit->UI()->AddText(L"GAME EXIT", 85.f, 0.f, 32, FONT_RGBA(0, 0, 0, 188));
	Exit->UI()->SetRectSize(320.f, 50.f);
	Exit->UI()->SetRectPos(0.f, -100.f);

	Exit->AddComponent(new PauseUIScript([]() { CGameMgr::GetInst()->ExitGame(); }));

	PauseMenuCanvasUI->AddChild(Exit);

	// "Option Menu Canvas" 
	CGameObject* OptionMenuCanvasUI = new CGameObject;
	OptionMenuCanvasUI->SetName(L"Option_CanvasUI");
	OptionMenuCanvasUI->AddComponent(new CUI(UI_CANVAS));

	OptionMenuCanvasUI->AddComponent(new CUIRender);
	OptionMenuCanvasUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.5f));
	OptionMenuCanvasUI->UI()->SetPriority(1);
	OptionMenuCanvasUI->UI()->SetRectPos(0.f, 0.f);
	OptionMenuCanvasUI->UI()->SetRectSize(1280.f, 768.f);

	SetObjectActive(OptionMenuCanvasUI, false);

	PLevel->AddObject(8, OptionMenuCanvasUI, false); // UI layer

	// Option Title
	CGameObject* OptionTitle = new CGameObject;
	OptionTitle->AddComponent(new CUI);

	OptionTitle->AddComponent(new CUIRender);
	OptionTitle->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 1.f));
	OptionTitle->SetName(L"OptionTitle");
	OptionTitle->UI()->ClearText();
	OptionTitle->UI()->AddText(L"O P T I O N", 500.f, -5.f, 45, FONT_RGBA(255, 255, 255, 255));
	OptionTitle->UI()->SetRectSize(1280.f, 100.f);
	OptionTitle->UI()->SetRectPos(0.f, 315.f);

	OptionTitle->AddComponent(new OptionUIScript);

	OptionMenuCanvasUI->AddChild(OptionTitle);

	// Drag bar & Cur Sensi
	CGameObject* Sensi = new CGameObject;
	Sensi->AddComponent(new CUI(UI_DRAG));

	Sensi->AddComponent(new CUIRender);
	Sensi->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));
	Sensi->SetName(L"SensiUI");
	Sensi->UI()->ClearText();
	Sensi->UI()->AddText(L"Cur Sensi : ", -75.f, 0.f, 20, FONT_RGBA(255, 255, 255, 255));
	Sensi->UI()->AddText(L" 0 ", 50.f, 0.f, 20, FONT_RGBA(255, 255, 255, 255));
	Sensi->UI()->SetRectSize(320.f, 50.f);
	Sensi->UI()->SetRectPos(0.f, 160.f);

	Sensi->AddComponent(new OptionUIScript);

	OptionMenuCanvasUI->AddChild(Sensi);

	// Up button
	CGameObject* Upbtn = new CGameObject;
	Upbtn->AddComponent(new CUI(UI_CLICK | UI_HOVER));

	Upbtn->AddComponent(new CUIRender);
	Upbtn->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	Upbtn->SetName(L"Upbtn");
	Upbtn->UI()->ClearText();
	Upbtn->UI()->AddText(L">", 0.f, -8.f, 20, FONT_RGBA(0, 0, 0, 188));
	Upbtn->UI()->SetRectSize(20.f, 20.f);
	Upbtn->UI()->SetRectPos(230.f, 170.f);

	Upbtn->AddComponent(new OptionUIScript([]() { CGameMgr::GetInst()->UpSensi(); }));

	OptionMenuCanvasUI->AddChild(Upbtn);

	// Down button
	CGameObject* Dwnbtn = new CGameObject;
	Dwnbtn->AddComponent(new CUI(UI_CLICK | UI_HOVER));

	Dwnbtn->AddComponent(new CUIRender);
	Dwnbtn->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	Dwnbtn->SetName(L"Dwnbtn");
	Dwnbtn->UI()->ClearText();
	Dwnbtn->UI()->AddText(L"<", 0.f, -8.f, 20, FONT_RGBA(0, 0, 0, 188));
	Dwnbtn->UI()->SetRectSize(20.f, 20.f);
	Dwnbtn->UI()->SetRectPos(200.f, 170.f);

	Dwnbtn->AddComponent(new OptionUIScript([]() { CGameMgr::GetInst()->DownSensi(); }));

	OptionMenuCanvasUI->AddChild(Dwnbtn);

	// Cancel button
	CGameObject* ExitOption = new CGameObject;
	ExitOption->AddComponent(new CUI(UI_CLICK | UI_HOVER));

	ExitOption->AddComponent(new CUIRender);
	ExitOption->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	ExitOption->SetName(L"ExitOption");
	ExitOption->UI()->ClearText();
	ExitOption->UI()->AddText(L"Exit", 0.f, 0.f, 32, FONT_RGBA(0, 0, 0, 188));
	ExitOption->UI()->SetRectSize(50.f, 50.f);
	ExitOption->UI()->SetRectPos(0.f, -180.f);

	ExitOption->AddComponent(new OptionUIScript([]() { CGameMgr::GetInst()->ExitOption(); }));

	OptionMenuCanvasUI->AddChild(ExitOption);

	// "Main Canvas UI" : 화면 전체 가리는 투명 ui
	CGameObject* MainCanvasUI = new CGameObject;
	MainCanvasUI->SetName(L"Main_CanvasUI");
	MainCanvasUI->AddComponent(new CUI(UI_CANVAS));
	MainCanvasUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));
	MainCanvasUI->UI()->SetPriority(4);
	MainCanvasUI->UI()->SetRectPos(0.f, 0.f);
	MainCanvasUI->UI()->SetRectSize(1280.f, 768.f);

	MainCanvasUI->AddComponent(new CUIRender);

	PLevel->AddObject(8, MainCanvasUI, false);

	// HP UI
	CGameObject* childUI = new CGameObject;
	childUI->SetName(L"HP_UI");
	childUI->AddComponent(new CUI);
	childUI->UI()->SetRectPos(Vec2(0.f, -350.f));
	childUI->UI()->SetRectSize(Vec2(280.f, 18.f));

	childUI->AddComponent(new CUIRender);
	childUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.4f));

	childUI->UIRender()->SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"UIHPMtrl"), 0);

	MainCanvasUI->AddChild(childUI);

	// Timer UI
	childUI = new CGameObject;
	childUI->SetName(L"Timer_UI");
	childUI->AddComponent(new CUI);
	childUI->UI()->SetRectPos(Vec2(0.f, 0.f));
	childUI->UI()->SetRectSize(Vec2(60.f, 60.f));

	childUI->AddComponent(new CUIRender);
	childUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.6f));

	childUI->UIRender()->SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"UIItemUseMtrl"), 0);
	childUI->UI()->AddText(L"", 17.f, 16.f, 20, FONT_RGBA(255, 255, 255, 255));
	SetObjectActive(childUI, false);

	MainCanvasUI->AddChild(childUI);

	// Interaction UI
	CGameObject* interactionUI = new CGameObject;
	interactionUI->SetName(L"Interaction_UI");
	interactionUI->AddComponent(new CUI);
	interactionUI->UI()->SetRectPos(Vec2(100.f, -50.f));
	interactionUI->UI()->SetRectSize(Vec2(80.f, 30.f));

	interactionUI->AddComponent(new CUIRender);
	interactionUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.6f));

	interactionUI->UIRender()->SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
	interactionUI->UI()->AddText(L"취소", 40.f, 4.f, 16, FONT_RGBA(255, 255, 255, 255));
	SetObjectActive(interactionUI, false);

	MainCanvasUI->AddChild(interactionUI);

	// Interaction Key UI
	childUI = new CGameObject;
	childUI->SetName(L"InteractionKey_UI");
	childUI->AddComponent(new CUI);
	childUI->UI()->SetRectPos(Vec2(-25.f, 0.f));
	childUI->UI()->SetRectSize(Vec2(30.f, 30.f));

	childUI->AddComponent(new CUIRender);
	childUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.6f));

	childUI->UIRender()->SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
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

	RoundsUI->UIRender()->SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
	RoundsUI->AddComponent(new RoundsUIScript);

	MainCanvasUI->AddChild(RoundsUI);

	// Kill Info
	CGameObject* KillinfoUI = new CGameObject;
	KillinfoUI->SetName(L"Killinfo_UI");
	KillinfoUI->AddComponent(new CUI);
	KillinfoUI->UI()->SetRectPos(Vec2(0.f, -200.f));
	KillinfoUI->UI()->SetRectSize(Vec2(500.f, 40.f));

	KillinfoUI->AddComponent(new CUIRender);
	KillinfoUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));

	KillinfoUI->UIRender()->SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
	KillinfoUI->AddComponent(new KillinfoUIScript);

	MainCanvasUI->AddChild(KillinfoUI);

	// 크로스헤어 UI
	childUI = new CGameObject;
	childUI->SetName(L"CrosshairUI");
	childUI->AddComponent(new CUI);
	childUI->UI()->SetRectPos(Vec2(0.f, 0.f));
	childUI->UI()->SetRectSize(Vec2(70.f, 70.f));
	childUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));
	childUI->UI()->SetImage(FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\CrossHair_1.png"));

	childUI->AddComponent(new CUIRender);
	childUI->UIRender()->SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"UICrosshairMtrl"), 0);
	childUI->UIRender()->GetMaterial(0)->SetScalarParam(INT_0, 1);
	childUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, 0.08f);
	childUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, 0.03f);
	childUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_2, 0.2f);
	childUI->UIRender()->GetMaterial(0)->SetScalarParam(VEC4_1, Vec4(0.0f, 1.0f, 0.0f, 1.0f));

	MainCanvasUI->AddChild(childUI);

	// Restart UI
	CGameObject* RestartUI = new CGameObject;
	RestartUI->SetName(L"Restart_UI");
	RestartUI->AddComponent(new CUI);
	RestartUI->UI()->SetRectPos(Vec2(0.f, 40.f));
	RestartUI->UI()->SetRectSize(Vec2(900.f, 40.f));
	RestartUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));

	RestartUI->AddComponent(new CUIRender);
	RestartUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));

	RestartUI->UIRender()->SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
	RestartUI->UI()->AddText(L"Press \"R\" To Restart", 200.f, 0.f, 62, FONT_RGBA(255, 255, 255, 255));

	SetObjectActive(RestartUI, false);
	MainCanvasUI->AddChild(RestartUI);

	// "Cardinal Direction Canvas UI"
	CGameObject* CardinalCanvasUI = new CGameObject;
	CardinalCanvasUI->SetName(L"Cardinal_CanvasUI");
	CardinalCanvasUI->AddComponent(new CUI(UI_CANVAS));
	CardinalCanvasUI->UI()->SetRectPos(Vec2(0.f, 320.f));
	CardinalCanvasUI->UI()->SetRectSize(Vec2(560.f, 55.f));

	CardinalCanvasUI->AddComponent(new CUIRender);
	CardinalCanvasUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.1f));
	CardinalCanvasUI->UI()->SetPriority(3);

	PLevel->AddObject(8, CardinalCanvasUI, false);

	CGameObject* pImageUI = new CGameObject;
	pImageUI->SetName(L"Cardinal_ImageUI");
	pImageUI->AddComponent(new CUI);
	pImageUI->UI()->SetImage(
		FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\Cardinal.png", L"Texture\\UI\\Cardinal.png"));
	pImageUI->UI()->SetRectSize(Vec2(1140.f, 48.f));
	pImageUI->UI()->SetRectPos(Vec2(0.f, -7.f));

	pImageUI->AddComponent(new CUIRender);
	pImageUI->UIRender()->SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"UICardinalMtrl"), 0);

	CardinalCanvasUI->AddChild(pImageUI);

	pImageUI = new CGameObject;
	pImageUI->SetName(L"Cardinal_ArrowUI");
	pImageUI->AddComponent(new CUI);
	pImageUI->UI()->SetImage(
		FAssetManager::GetInst()->Load<CTexture>(L"Texture\\UI\\Cardinal_Arrow.png",
		                                         L"Texture\\UI\\Cardinal_Arrow.png"));
	pImageUI->UI()->SetRectSize(Vec2(15.f, 15.f));
	pImageUI->UI()->SetRectPos(Vec2(0.f, 20.f));

	pImageUI->AddComponent(new CUIRender);

	CardinalCanvasUI->AddChild(pImageUI);

	// ==========
	// MiniMapCamera
	// ==========
	CGameObject* MinimapCamera = new CGameObject;
	MinimapCamera->SetName(L"MinimapCamera");
	MinimapCamera->AddComponent(new CCamera);
	MinimapCamera->Camera()->SetProjType(ORTHOGRAPHIC);
	MinimapCamera->Camera()->SetPriority(1);
	MinimapCamera->Camera()->SetScale(13.0);

	MinimapCamera->Camera()->LayerCheckClear();
	//MinimapCamera->Camera()->LayerOn(0); // Background
	MinimapCamera->Camera()->LayerOn(1); // Structure

	MinimapCamera->AddComponent(new MinimapCameraScript);

	// 임시 배치
	MinimapCamera->Transform()->SetRelativePos(Vec3(0, 7000, 0));
	MinimapCamera->Transform()->SetRelativeRotation(Vec3(90, 0, 0));

	PLevel->AddObject(0, MinimapCamera, false);

	// MinimapUI
	CGameObject* pMinimapUI = new CGameObject;
	pMinimapUI->SetName(L"MinimapUI");
	pMinimapUI->AddComponent(new CUI);
	pMinimapUI->AddComponent(new CUIRender);
	pMinimapUI->UIRender()->SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"UIMinimapMtrl"), 0);

	Ptr<CTexture> pMinimapTex = FAssetManager::GetInst()->FindAsset<CTexture>(L"MinimapTargetTex");
	pMinimapUI->UI()->SetImage(pMinimapTex);
	pMinimapUI->UI()->SetRectPos(Vec2(450.f, -200.f));
	pMinimapUI->UI()->SetRectSize(Vec2(300.f, 300.f));
	pMinimapUI->UI()->SetColor(Vec4(0.2f, 0.2f, 0.2f, 0.7f));

	pMinimapUI->AddComponent(new MinimapUIScript);

	MainCanvasUI->AddChild(pMinimapUI);

	return {Vicinity, Inventory, interactionUI};
}

// TODO(KHJ): FBX 기반 Common Load Method 구축할 것

void TestLevel::SetUpPlayer(CLevel* PLevel, const vector<CGameObject*>& PUIInfo)
{
	auto* Player = GameFactory::LoadDefaultPlayer(PLevel, {4000.f, 600.f, 1500.f});
	Player->AddComponent(new PlayerCharacter);
	Player->AddComponent(new InventoryController);

	InventoryController* InventoryScript = static_cast<InventoryController*>(GetScriptWithType(
		Player, SCRIPT_TYPE::INVENTORYSCRIPT));

	// 주변 및 인벤토리 UI를 등록함
	InventoryScript->SetVicinityUI(PUIInfo[0]);
	InventoryScript->SetInventoryUI(PUIInfo[1]);
	InventoryScript->SetPlayer(Player);

	// Interaction Object
	CGameObject* pInteractionHandler = new CGameObject;
	pInteractionHandler->SetName(L"Interaction Handler");

	pInteractionHandler->AddComponent(new FSphereCollider);
	Engine::Collider::SetColliderDynamic(pInteractionHandler, EColliderType::SphereCollider);
	pInteractionHandler->SphereCollider()->SetIndependent(1000.f);

	auto pHandlerScript = new InteractionHandler;

	pHandlerScript->SetPlayer(Player);
	pHandlerScript->SetInteractionUI(PUIInfo[2]);

	pInteractionHandler->AddComponent(pHandlerScript);

	Player->AddChild(pInteractionHandler);
}

void TestLevel::SetUpEvent(CLevel* PLevel)
{
	// FadeInOut테스트용
	CGameObject* FadeInOutEvent = new CGameObject;
	FadeInOutEvent->SetName(L"FadeInOut_Event");
	FadeInOutEvent->AddComponent(new TestFadeInOutReset);

	PLevel->AddObject(8, FadeInOutEvent, false);

	// Player Revive
	CGameObject* PlayerReviveEvent = new CGameObject;
	PlayerReviveEvent->SetName(L"PlayerRevive_Event");
	PlayerReviveEvent->AddComponent(new PlayerRevive);

	PLevel->AddObject(8, PlayerReviveEvent, false);

	// RestartLevel
	CGameObject* LevelRestartEvent = new CGameObject;
	LevelRestartEvent->SetName(L"LevelRestart_Event");
	LevelRestartEvent->AddComponent(new LevelRestart);

	PLevel->AddObject(8, LevelRestartEvent, false);
}
