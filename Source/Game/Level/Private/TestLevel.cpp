#include "pch.h"
#include "Game/Level/Public/TestLevel.h"

#include "Engine/API/Public/EngineFacade.h"

#include "Game/Factory/Public/GameFactory.h"
#include "Game/Gameplay/Inventory/Public/Item.h"
#include "Game/Gameplay/Inventory/Public/UI_Inventory.h"
#include "Game/Gameplay/Inventory/Public/UI_Item.h"
#include "Game/Gameplay/Inventory/Public/UI_Vicinity.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/UI/Public/KillinfoUIScript.h"
#include "Game/Gameplay/UI/Public/RoundsUIScript.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"
#include "Game/Gameplay/Weapon/Public/ThrowableController.h"
#include "Game/Gameplay/Character/Public/TestCharacter.h"
#include "Game/Gameplay/Character/Public/EnemyVisionScript.h"
#include "Game/Gameplay/Character/Public/InteractionHandler.h"
#include "Game/Gameplay/Door/Public/DoorScript.h"
#include "Game/Gameplay/UI/Public/MinimapUIScript.h"
#include "Game/Gameplay/UI/Public/MinimapCameraScript.h"
#include "Game/Gameplay/Character/Public/CameraEffect.h"

// TODO(KHJ): 이하 헤더 배제 시도
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Physics/Collider3D.h"
#include "Engine/Runtime/Public/Component/Physics/ColliderRay.h"
#include "Engine/Runtime/Public/Component/Physics/MeshCollider.h"
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
	// GameFactory::LoadDefaultLandscape(LevelRawPtr);

	// Initialize Item Parts
	auto UIInfo = SetUpUI(LevelRawPtr);
	ItemMgr::GetInst()->Init();

	SetUpPlayer(LevelRawPtr, UIInfo);

	// Weapon
	GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\ak47_test.fbx",
		L"AKM",
		Vec3(90.f, 0.f, 30.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(700.f, 700.f, 700.f),
		{
			[](CGameObject* obj)
			{
				Engine::Common::AddComponentToObject<FColliderRay>(obj);
				Engine::Common::AddComponentToObject<FCollider3D>(obj);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::Collider3D);
			},
			[](CGameObject* obj)
			{
				Engine::Common::AddScriptToObject<GunController>(obj);
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
		Vec3(0.f, 0.f, 5000.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(10.f, 10.f, 10.f),
		{
			[](CGameObject* obj)
			{
				Engine::Common::AddComponentToObject<FCollider3D>(obj);
				// TODO(KHJ): Wrapping
				obj->Collider3D()->SetName(L"Weapon");
				obj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->Collider3D()->SetIndependentScale(true);
				obj->Collider3D()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::Collider3D);
			},
			[](CGameObject* obj)
			{
				Engine::Common::AddScriptToObject<ThrowableController>(obj);
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
		Vec3(0.f, 0.f, 10000.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(10.f, 10.f, 10.f),
		{
			[](CGameObject* obj)
			{
				Engine::Common::AddComponentToObject<FCollider3D>(obj);
				// TODO(KHJ): Wrapping
				obj->Collider3D()->SetName(L"Weapon");
				obj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->Collider3D()->SetIndependentScale(true);
				obj->Collider3D()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::Collider3D);
			},
			[](CGameObject* obj)
			{
				Engine::Common::AddScriptToObject<ThrowableController>(obj);
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
				Engine::Common::AddComponentToObject<FCollider3D>(obj);
				// TODO(KHJ): Wrapping
				obj->Collider3D()->SetName(L"Weapon");
				obj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->Collider3D()->SetIndependentScale(true);
				obj->Collider3D()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::Collider3D);
			},
			[](CGameObject* obj)
			{
				Engine::Common::AddScriptToObject<GunController>(obj);
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
				Engine::Common::AddComponentToObject<FCollider3D>(obj);
				// TODO(KHJ): Wrapping
				obj->Collider3D()->SetName(L"Weapon");
				obj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->Collider3D()->SetIndependentScale(true);
				obj->Collider3D()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::Collider3D);
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
				Engine::Common::AddComponentToObject<FCollider3D>(obj);
				// TODO(KHJ): Wrapping
				obj->Collider3D()->SetName(L"Weapon");
				obj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->Collider3D()->SetIndependentScale(true);
				obj->Collider3D()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::Collider3D);
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
				Engine::Common::AddComponentToObject<FCollider3D>(obj);
				// TODO(KHJ): Wrapping
				obj->Collider3D()->SetName(L"Weapon");
				obj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
				obj->Collider3D()->SetIndependentScale(true);
				obj->Collider3D()->SetTrigger(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::Collider3D);
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
				Engine::Common::AddComponentToObject<FCollider3D>(obj);
				// TODO(KHJ): Wrapping
				Engine::Collider::SetColliderProperties(obj, {1000, 500, 1000}, {0, 50, 0}, true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::Collider3D);
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
	pVision->AddComponent(new FColliderRay);
	pVision->AddComponent(new EnemyVisionScript);
	pVision->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));

	//pVision->AddComponent(new FCollider3D);
	//pVision->Collider3D()->SetScale(Vec3(1000.f, 1000.f, 4000.f));
	//pVision->Collider3D()->SetOffset(Vec3(0.f, 0.f, 2500.f));
	//pVision->Collider3D()->SetIndependentScale(true);
	//pVision->Collider3D()->SetTrigger(true);

	pVision->ColliderRay()->SetRayLength(1000);
	pVision->ColliderRay()->SetIndependentDir(true);
	pVision->ColliderRay()->SetTriggerTarget(false);

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
				obj->AddComponent(new FCollider3D);
				// TODO(KHJ): Wrapping
				obj->Collider3D()->SetScale(Vec3(650.f, 1600.f, 40.f));
				obj->Collider3D()->SetOffset(Vec3(310.f, 800.f, 0.f));
				obj->Collider3D()->SetIndependentScale(true);
				Engine::Collider::SetColliderDynamic(obj, EColliderType::Collider3D);
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

	// Temp Landscape
	// GameFactory::MakeFBXObject(
	// 	LevelRawPtr,
	// 	L"FBX\\Props\\Death Box\\box.fbx",
	// 	L"TempLandscape",
	// 	Vec3(0.f, -800.f, 0.f),
	// 	Vec3(0.f, 0.f, 0.f),
	// 	Vec3(20000.f, 200.f, 20000.f),
	// 	{
	// 		[](CGameObject* obj)
	// 		{
	// 			obj->AddComponentRecursive<FMeshCollider>();
	// 			obj->MeshCollider()->SetUseOriginalMesh(true);
	// 			obj->Transform()->SetFrustumCheck(false);
	// 		}
	// 	},
	// 	1,
	// 	true
	// );

	GameFactory::MakeFBXObject(
		LevelRawPtr,
		L"FBX\\Downtown_Alley_Scene.fbx",
		L"Downtown_Alley",
		Vec3(0.f, -1500.f, 0.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(1.f, 1.f, 1.f), // 기본 스케일, 아래에서 Multiply로 조정
		{
			[](CGameObject* obj)
			{
				obj->Transform()->SetRelativeScaleMultiply(4.f);
				obj->AddComponentRecursive<FMeshCollider>();
			}
		},
		1,
		true
	);
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

	PLevel->AddObject(8, CanvasUI, false); // UI layer

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

	// Reload UI
	childUI = new CGameObject;
	childUI->SetName(L"Reload_UI");
	childUI->AddComponent(new CUI);
	childUI->UI()->SetRectPos(Vec2(0.f, 0.f));
	childUI->UI()->SetRectSize(Vec2(60.f, 60.f));

	childUI->AddComponent(new CUIRender);
	childUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.6f));

	childUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIItemUseMtrl"), 0);
	childUI->UI()->AddText(L"", 17.f, 16.f, 20, FONT_RGBA(255, 255, 255, 255));
	SetObjectActive(childUI, false);

	CanvasUI->AddChild(childUI);

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


	// PostProcess
	CGameObject* pCameraPost = new CGameObject;
	pCameraPost->SetName(L"CameraPost");
	pCameraPost->AddComponent(new CUI);
	pCameraPost->UI()->SetRectPos(Vec2(0.f, 0.f));
	pCameraPost->UI()->SetRectSize(Vec2(1280.f, 768.f));
	pCameraPost->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));
	pCameraPost->AddComponent(new CUIRender);
	pCameraPost->UIRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pCameraPost->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"CameraPostMtrl"), 0);
	pCameraPost->AddComponent(new CameraEffect);

	CanvasUI->AddChild(pCameraPost);

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
	pImageUI->UI()->SetImage(
		CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\UI\\Cardinal.png", L"Texture\\UI\\Cardinal.png"));
	pImageUI->UI()->SetRectSize(Vec2(1140.f, 48.f));
	pImageUI->UI()->SetRectPos(Vec2(0.f, -7.f));

	pImageUI->AddComponent(new CUIRender);
	pImageUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UICardinalMtrl"), 0);

	CanvasUI->AddChild(pImageUI);

	pImageUI = new CGameObject;
	pImageUI->SetName(L"Cardinal_ArrowUI");
	pImageUI->AddComponent(new CUI);
	pImageUI->UI()->SetImage(
		CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\UI\\Cardinal_Arrow.png", L"Texture\\UI\\Cardinal_Arrow.png"));
	pImageUI->UI()->SetRectSize(Vec2(15.f, 15.f));
	pImageUI->UI()->SetRectPos(Vec2(0.f, 20.f));

	pImageUI->AddComponent(new CUIRender);

	CanvasUI->AddChild(pImageUI);

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

	//Minimap CanvasUI
	CGameObject* MapCanvasUI = new CGameObject;
	MapCanvasUI->SetName(L"MiniMap_CanvasUI");
	MapCanvasUI->AddComponent(new CUI(UI_CANVAS));

	MapCanvasUI->AddComponent(new CUIRender);
	MapCanvasUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.f));
	MapCanvasUI->UI()->SetPriority(0);
	MapCanvasUI->UI()->SetRectPos(485.f, -230.f);
	MapCanvasUI->UI()->SetRectSize(300.f, 300.f);

	PLevel->AddObject(8, MapCanvasUI, false);

	// MinimapUI
	CGameObject* pMinimapUI = new CGameObject;
	pMinimapUI->SetName(L"MinimapUI");
	pMinimapUI->AddComponent(new CUI);
	pMinimapUI->AddComponent(new CUIRender);
	pMinimapUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMinimapMtrl"), 0);

	Ptr<CTexture> pMinimapTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"MinimapTargetTex");
	pMinimapUI->UI()->SetImage(pMinimapTex);
	pMinimapUI->UI()->SetRectPos(Vec2(0.f, 0.f));
	pMinimapUI->UI()->SetRectSize(Vec2(300.f, 300.f));
	pMinimapUI->UI()->SetColor(Vec4(0.2f, 0.2f, 0.2f, 0.7f));

	pMinimapUI->AddComponent(new MinimapUIScript);

	MapCanvasUI->AddChild(pMinimapUI);

	return {Vicinity, Inventory, interactionUI};
}

// TODO(KHJ): FBX 기반 Common Load Method 구축할 것

void TestLevel::SetUpPlayer(CLevel* PLevel, const vector<CGameObject*>& PUIInfo)
{
	auto* Player = GameFactory::LoadDefaultPlayer(PLevel, {4000.f, 500.f, 1500.f});
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
	pInteractionHandler->AddComponent(new FCollider3D);
	Engine::Collider::SetColliderDynamic(pInteractionHandler, EColliderType::Collider3D);

	pInteractionHandler->Collider3D()->SetScale(Vec3(2000.f, 2000.f, 2000.f));
	pInteractionHandler->Collider3D()->SetIndependentScale(true);
	pInteractionHandler->Collider3D()->SetTrigger(true);
	pInteractionHandler->Collider3D()->SetOffset(Vec3(0.f, 1000.f, 0.f));

	auto pHandlerScript = new InteractionHandler;

	pHandlerScript->SetPlayer(Player);
	pHandlerScript->SetInteractionUI(PUIInfo[2]);

	pInteractionHandler->AddComponent(pHandlerScript);

	Player->AddChild(pInteractionHandler);
}
