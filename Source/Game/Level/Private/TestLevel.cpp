#include "pch.h"
#include "Game/Level/Public/TestLevel.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Light/CLight3D.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Engine/Runtime/Public/Component/Rendering/CSkyBox.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CCollisionMgr.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"
#include "Engine/System/Public/Manager/CObjectPoolMgr.h"
#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"
#include "Game/Gameplay/Weapon/Public/ThrowableController.h"
#include "Game/Gameplay/TestSound.h"
#include "Game/Gameplay/Character/Public/EnemyVisionScript.h"
#include "Game/Gameplay/Character/Public/InteractionHandler.h"
#include "Game/Gameplay/Character/Public/TestCharacter.h"
#include "Game/Gameplay/Particle/Public/ParticleController.h"

#include "Engine/Runtime/Public/Component/Rendering/CParticleSystem.h"

#include "Game/GamePlay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Inventory/Public/Item.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"
#include "Game/Gameplay/Inventory/Public/UI_Item.h"
#include "Game/Gameplay/Inventory/Public/UI_Vicinity.h"
#include "Game/Gameplay/Inventory/Public/UI_Inventory.h"
#include "Game/Gameplay/Door/Public/DoorScript.h"

void TestLevel::CreateTestLevel()
{
	// Texture 로딩하기
	Ptr<CTexture> pTex = CAssetMgr::GetInst()->Load<CTexture>(
		L"PlayerTex", L"Texture\\Character.png");
	Ptr<CTexture> pAtlasTex = CAssetMgr::GetInst()->Load<CTexture>(
		L"TileMapTex", L"Texture\\TILE.bmp");

	CLevel* pLevel = new CLevel;

	// 테스트 레벨을 현재 레벨로 지정
	ChangeLevel(pLevel, LEVEL_STATE::STOP);

	pLevel->GetLayer(0)->SetName(L"Background");
	pLevel->GetLayer(1)->SetName(L"Structure");	// 건물, 구조물 등
	pLevel->GetLayer(2)->SetName(L"Default");
	pLevel->GetLayer(3)->SetName(L"PlayerTPS");
	pLevel->GetLayer(4)->SetName(L"PlayerFPS");
	pLevel->GetLayer(5)->SetName(L"PlayerObject");
	pLevel->GetLayer(6)->SetName(L"Item");
	pLevel->GetLayer(7)->SetName(L"MonsterObject");
	pLevel->GetLayer(8)->SetName(L"UI");
	pLevel->GetLayer(9)->SetName(L"ObjectPool");

	// 충돌 설정
	CCollisionMgr::GetInst()->CollisionCheck(0, 0);
	CCollisionMgr::GetInst()->CollisionCheck(3, 0);
	CCollisionMgr::GetInst()->CollisionCheck(3, 6);
	CCollisionMgr::GetInst()->CollisionCheck(3, 1);
	CCollisionMgr::GetInst()->CollisionCheck(3, 7);
	CCollisionMgr::GetInst()->CollisionCheck(0, 7);

	CGameObject* pObject = nullptr;

	// ==========
	// MainCamera
	// ==========
	pObject = new CGameObject;
	pObject->SetName(L"MainCamera");
	pObject->AddComponent(new CCamera);
	pObject->AddComponent(new CameraController);

	pObject->Camera()->SetProjType(PERSPECTIVE);
	pObject->Camera()->SetPriority(0);
	pObject->Camera()->LayerCheckAll();
	pObject->Camera()->LayerCheck(4);

	pLevel->AddObject(0, pObject, false);

	// =================
	// 광원 오브젝트 추가
	// =================
	auto pLightObj = new CGameObject;
	pLightObj->SetName(L"Point Light");
	pLightObj->AddComponent(new CLight3D);
	pLightObj->Transform()->SetRelativePos(0.f, -450.f, 0.f);
	pLightObj->Transform()->SetRelativeRotation(45.f, 45.f, 0.f);
	pLightObj->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pLightObj->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pLightObj->Light3D()->SetAmbient(Vec3(0.15f, 0.15f, 0.15f));
	pLightObj->Light3D()->SetSpecularCoefficient(0.3f);
	pLightObj->Light3D()->SetRadius(300.f);
	pLevel->AddObject(0, pLightObj, false);

	// ======
	// SkyBox
	// ======
	auto pSkyBox = new CGameObject;
	pSkyBox->SetName(L"SkyBox");
	pSkyBox->AddComponent(new CSkyBox);
	Ptr<CTexture> pSkyBoxTex = CAssetMgr::GetInst()->FindAsset<CTexture>(
		L"Texture\\skybox\\Sky01.png");

	// FrustumCheck 비활성화
	pSkyBox->Transform()->SetFrustumCheck(false);

	pSkyBox->SkyBox()->SetMode(SPHERE);
	pSkyBox->SkyBox()->SetSkyBoxTexture(pSkyBoxTex);
	pLevel->AddObject(0, pSkyBox, false);

	// =========
	// LandScape
	// =========
	auto pLandScape = new CGameObject;
	pLandScape->SetName(L"LandScape");
	pLandScape->AddComponent(new CLandScape);

	pLandScape->Transform()->SetRelativePos(Vec3(0.f, -500.f, 0.f));
	pLandScape->Transform()->SetRelativeScale(Vec3(500.f, 500.f, 500.f));

	pLandScape->LandScape()->SetFace(32, 32);
	pLandScape->LandScape()->CreateHeightMap(1024, 1024);
	//pLandScape->LandScape()->SetHeightMapTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\HeightMap\\HeightMap_01.jpg"));
	pLandScape->LandScape()->SetColorTexture(
		CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\LandScapeTexture\\LS_Color.dds"));
	pLandScape->LandScape()->SetNormalTexture(
		CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\LandScapeTexture\\LS_Normal.dds"));

	pLevel->AddObject(0, pLandScape, false);


	// =========
	// UI Preset
	// =========
	
	// UI Camera
	CGameObject* UICamera = new CGameObject;
	UICamera->SetName(L"UICamera");
	UICamera->AddComponent(new CCamera);
	UICamera->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	UICamera->Camera()->SetPriority(1);
	UICamera->Camera()->SetFar(10.f);	// 1-10 까지 UI 계층 (Canvas)

	assert(pLevel->GetLayer(8)->GetName() == L"UI");
	UICamera->Camera()->LayerCheck(8);

	pLevel->AddObject(0, UICamera, false);

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

	pLevel->AddObject(8, CanvasUI, false);	// UI layer

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
		ChildUI->UI()->SetImage(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\Idle_Left.bmp"));

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
		ChildUI->UI()->SetImage(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\Idle_Left.bmp"));

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

	pLevel->AddObject(8, CanvasUI, false);

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

	pLevel->AddObject(8, CanvasUI, false);
	
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

	


	// 아이템 매니저 Initialize
	ItemMgr::GetInst()->Init();


	// ============
	// FBX Loading
	// ============
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;


		//pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\AK_ANIMACION.fbx");
		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\pubg_test2.fbx");
		//pMeshData = CAssetMgr::GetInst()->FindAsset<CMeshData>(L"MeshData\\Monster.mdat");

		Ptr<CMeshData> pWeaponModel = CAssetMgr::GetInst()->LoadFBX(L"FBX\\ak47_test.fbx");

		int modelCnt = 1;
		for (int i = 0; i < modelCnt; ++i)
		{
			pObj = pMeshData->Instantiate();
			pObj->SetName(L"Player");
			pObj->AddComponent(new CCollider3D);
			pObj->AddComponent(new CColliderRay);

			pObj->Transform()->SetRelativePos(Vec3(500.f + i * 200.f, -380.f, 500.f));
			pObj->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));
			pObj->Transform()->SetRelativeRotation(0.f, 90.f, 0.f);

			pObj->ColliderRay()->SetRayDir(Vec3(0.f, 0.f, -1.f));
			pObj->ColliderRay()->SetOffset(Vec3(0.f, 1500.f, 0.f));
			pObj->ColliderRay()->SetRayLength(5000.f);
			pObj->ColliderRay()->SetTriggerTarget(true);

			pObj->AddComponent(new PlayerCharacter);
			pObj->Collider3D()->SetScale(Vec3(1000.f, 1000.f, 1000.f));
			pObj->Collider3D()->SetIndependentScale(true);

			pObj->AddComponent(new InventoryController);
			InventoryController* pInvenScript = static_cast<InventoryController*>(pObj->GetScript(INVENTORYSCRIPT));

			// 주변 및 인벤토리 UI를 등록함
			pInvenScript->SetVicinityUI(Vicinity);
			pInvenScript->SetInventoryUI(Inventory);
			pInvenScript->SetPlayer(pObj);

			pObj->Animator3D()->SetClipTime(0, 0.3f * i);

			// 자식 메쉬들 같이 이동
			pLevel->AddObject(3, pObj, true);

			// Interaction Object
			CGameObject* pInteractionHandler = new CGameObject;
			pInteractionHandler->SetName(L"Interaction Handler");
			pInteractionHandler->AddComponent(new CCollider3D);

			pInteractionHandler->Collider3D()->SetScale(Vec3(2000.f, 2000.f, 2000.f));
			pInteractionHandler->Collider3D()->SetIndependentScale(true);
			pInteractionHandler->Collider3D()->SetOffset(Vec3(0.f, 1000.f, 0.f));

			auto pHandlerScript = new InteractionHandler;

			pHandlerScript->SetPlayer(pObj);
			pHandlerScript->SetInteractionUI(interactionUI);

			pInteractionHandler->AddComponent(pHandlerScript);

			pObj->AddChild(pInteractionHandler);

			//
			// AKM
			//
			CGameObject* pWeaponObj = pWeaponModel->Instantiate();
			pWeaponObj->SetName(L"AKM");
			pWeaponObj->AddComponent(new CColliderRay);
			pWeaponObj->AddComponent(new CCollider3D);
			pWeaponObj->AddComponent(new GunController);

			WeaponController* pScript = static_cast<WeaponController*>(pWeaponObj->GetScripts()[0]);
			//pScript->SetEquippedOwner(pObj);
			pScript->SetWeaponType(WEAPON_TYPE::PRIMARY);

			pWeaponObj->Transform()->SetRelativePos(Vec3(90.f, 0.f, 30.f));
			pWeaponObj->Transform()->SetRelativeScale(Vec3(900.f, 900.f, 900.f));
			pWeaponObj->Transform()->SetRelativeRotation(0.f, -2.f, -4.3f);
			pWeaponObj->Transform()->SetIndependentScale(true);

			pWeaponObj->Collider3D()->SetName(L"Weapon");
			pWeaponObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
			pWeaponObj->Collider3D()->SetIndependentScale(true);
			pWeaponObj->Collider3D()->SetTrigger(true);

			pWeaponObj->ColliderRay()->SetRayPos(Vec3(-30.f, 100.f, 0.f));
			pWeaponObj->ColliderRay()->SetRayDir(Vec3(1.f, 0.f, 0.f));
			pWeaponObj->ColliderRay()->SetOffset(Vec3(556.f, 72.f, 0.f));

			pWeaponObj->AddComponent(new ItemScript(ITEM_TYPE::AKM));

			pLevel->AddObject(6, pWeaponObj, false);
			//PlayerCharacter* pPlayerScript = static_cast<PlayerCharacter*>(pObj->GetScripts()[0]);
			//pPlayerScript->SetCurWeapon(pWeaponObj);

			//pObj->AddChild(pWeaponObj);

			//
			// nade
			//
			pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Throwable\\GRENADE.fbx");
			pWeaponObj = pMeshData->Instantiate();
			pWeaponObj->SetName(L"Grenade");
			pWeaponObj->AddComponent(new CCollider3D);
			pWeaponObj->AddComponent(new ThrowableController);

			WeaponController* pScript1 = static_cast<WeaponController*>(pWeaponObj->GetScripts()[0]);
			pScript1->SetWeaponType(WEAPON_TYPE::THROWABLE);

			pWeaponObj->Collider3D()->SetName(L"Weapon");
			pWeaponObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
			pWeaponObj->Collider3D()->SetIndependentScale(true);
			pWeaponObj->Collider3D()->SetTrigger(true);

			pWeaponObj->Transform()->SetRelativePos(0.f, 0.f, 5000.f);
			pWeaponObj->Transform()->SetRelativeScale(Vec3(15.f, 15.f, 15.f));
			pWeaponObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
			pWeaponObj->Transform()->SetIndependentScale(true);

			pWeaponObj->AddComponent(new ItemScript(ITEM_TYPE::GRENADE));

			pLevel->AddObject(6, pWeaponObj, false);

			pLevel->AddObject(6, pWeaponObj->Clone(), false);

			//
			// smoke
			//
			pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Throwable\\M18_Smoke.fbx");
			pWeaponObj = pMeshData->Instantiate();
			pWeaponObj->SetName(L"Smoke Grenade");
			pWeaponObj->AddComponent(new CCollider3D);
			pWeaponObj->AddComponent(new ThrowableController);

			WeaponController* pScript2 = static_cast<WeaponController*>(pWeaponObj->GetScripts()[0]);
			pScript2->SetWeaponType(WEAPON_TYPE::THROWABLE);

			pWeaponObj->Collider3D()->SetName(L"Weapon");
			pWeaponObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
			pWeaponObj->Collider3D()->SetIndependentScale(true);
			pWeaponObj->Collider3D()->SetTrigger(true);

			pWeaponObj->Transform()->SetRelativePos(0.f, 0.f, 10000.f);
			pWeaponObj->Transform()->SetRelativeScale(Vec3(15.f, 15.f, 15.f));
			pWeaponObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
			pWeaponObj->Transform()->SetIndependentScale(true);

			pWeaponObj->AddComponent(new ItemScript(ITEM_TYPE::SMOKEBOMB));

			pLevel->AddObject(6, pWeaponObj, false);

			//
			// Test PrimaryWeapon
			//
			pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Heal\\Energy Drink.fbx");
			pWeaponObj = pMeshData->Instantiate();
			pWeaponObj->SetName(L"Energy Drink");
			pWeaponObj->AddComponent(new CCollider3D);
			pWeaponObj->AddComponent(new GunController);

			WeaponController* pScript3 = static_cast<WeaponController*>(pWeaponObj->GetScripts()[0]);
			pScript3->SetWeaponType(WEAPON_TYPE::SECONDARY);

			pWeaponObj->Collider3D()->SetName(L"Weapon");
			pWeaponObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
			pWeaponObj->Collider3D()->SetIndependentScale(true);
			pWeaponObj->Collider3D()->SetTrigger(true);

			pWeaponObj->Transform()->SetRelativePos(0.f, 0.f, 1000.f);
			pWeaponObj->Transform()->SetRelativeScale(Vec3(500.f, 500.f, 500.f));
			pWeaponObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);

			pWeaponObj->AddComponent(new ItemScript(ITEM_TYPE::ENERGY_DRINK));

			pLevel->AddObject(6, pWeaponObj, false);

			//
			// Test SecondaryWeapon
			//
			pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Heal\\First Aid Kit.fbx");
			pWeaponObj = pMeshData->Instantiate();
			pWeaponObj->SetName(L"First Aid Kit");
			pWeaponObj->AddComponent(new CCollider3D);
			pWeaponObj->AddComponent(new GunController);

			WeaponController* pScript4 = static_cast<WeaponController*>(pWeaponObj->GetScripts()[0]);
			pScript4->SetWeaponType(WEAPON_TYPE::PRIMARY);

			pWeaponObj->Collider3D()->SetName(L"Weapon");
			pWeaponObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
			pWeaponObj->Collider3D()->SetIndependentScale(true);
			pWeaponObj->Collider3D()->SetTrigger(true);

			pWeaponObj->Transform()->SetRelativePos(0.f, 0.f, 3000.f);
			pWeaponObj->Transform()->SetRelativeScale(Vec3(500.f, 500.f, 500.f));
			pWeaponObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);

			pWeaponObj->AddComponent(new ItemScript(ITEM_TYPE::FIRST_AID_KIT));

			pLevel->AddObject(6, pWeaponObj, false);
		}

		// 모바일 배그 애셋 테스트
		//pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\hG.fbx");
		//pObj = pMeshData->Instantiate();
		//pObj->SetName(L"Wall");
		//pObj->AddComponent(new CCollider3D);

		//pObj->Collider3D()->SetScale(Vec3(8.f, 5.f, 4.5f));
		//pObj->Collider3D()->SetRotY(13.31f);

		//pObj->Transform()->SetRelativePos(Vec3(3208.f, 0.f, -1066.f));
		//pObj->Transform()->SetRelativeScale(Vec3(500.f, 500.f, 500.f));
		//pObj->Transform()->SetRelativeRotation(0.f, 138.5f, 0.f);
		//pLevel->AddObject(0, pObj, false);

		//pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Air Drop\\DROP.fbx");
		//pObj = pMeshData->Instantiate();
		//pObj->SetName(L"Airdrop");

		//pObj->AddComponent(new CCollider3D);

		//pObj->Collider3D()->SetIndependentScale(true);
		//pObj->Collider3D()->SetScale(Vec3(2850.f, 2850.f, 2850.f));
		//pObj->Collider3D()->SetOffset(Vec3(0.f, 1500.f, 0.f));

		//pObj->Transform()->SetRelativePos(Vec3(2867.f, -494.f, 321.f));
		//pObj->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 50.f));
		//pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		//pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Heal\\Adrenaline Syringe.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Adrenaline");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);

		pObj->AddComponent(new CCollider3D);
		pObj->Collider3D()->SetName(L"Weapon");
		pObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
		pObj->Collider3D()->SetIndependentScale(true);
		pObj->Collider3D()->SetTrigger(true);

		pObj->AddComponent(new ItemScript(ITEM_TYPE::ADRENALINE_SYRINGE));

		pLevel->AddObject(6, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Heal\\Bandage.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Bandage");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);

		pObj->AddComponent(new CCollider3D);
		pObj->Collider3D()->SetName(L"Weapon");
		pObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
		pObj->Collider3D()->SetIndependentScale(true);
		pObj->Collider3D()->SetTrigger(true);

		pObj->AddComponent(new ItemScript(ITEM_TYPE::AMMO_5));

		pLevel->AddObject(6, pObj, false);

		pLevel->AddObject(6, pObj->Clone(), false);
		pLevel->AddObject(6, pObj->Clone(), false);
		pLevel->AddObject(6, pObj->Clone(), false);
		pLevel->AddObject(6, pObj->Clone(), false);
		pLevel->AddObject(6, pObj->Clone(), false);
		pLevel->AddObject(6, pObj->Clone(), false);
		pLevel->AddObject(6, pObj->Clone(), false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Death Box\\box.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"DeathBox");

		pObj->AddComponent(new CCollider3D);

		pObj->Collider3D()->SetIndependentScale(true);
		pObj->Collider3D()->SetScale(Vec3(3900.f, 3900.f, 2300.f));
		pObj->Collider3D()->SetOffset(Vec3(0.f, 1868.f, 0.f));

		pObj->Transform()->SetRelativePos(Vec3(6590.f, -410.f, 5000.f));
		pObj->Transform()->SetRelativeScale(Vec3(700.f, 2000.f, 700.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);

		pLevel->AddObject(1, pObj, false);


		/*
		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Heal\\Med Kit.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Med Kit");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Throwable\\Molotov.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Molotov");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Props\\Heal\\Painkiller.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Painkiller");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);
		*/

	}

	// 배경 사운드 테스트
	//Ptr<CSound> soundBGM = CAssetMgr::GetInst()->FindAsset<CSound>(L"Sound\\Menu_Theme.wav");
	//CSoundMgr::GetInst()->SetGameBGM(soundBGM, false);
	//CSoundMgr::GetInst()->PlayGameBGM(true, 0.5f, false);


	//// 적 총기 음성 테스트
	//pObject = new CGameObject;
	//pObject->SetName(L"TestSound");
	//pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CCollider3D);
	//pObject->AddComponent(new TestSound);

	//pObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
	//pObject->MeshRender()->SetMaterial(
	//	CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	//pObject->Transform()->SetRelativePos(Vec3(3000.f, 0.f, 1000.f));
	//pObject->Transform()->SetRelativeScale(Vec3(500.f, 500.f, 500.f));
	//pObject->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	//pObject->Transform()->SetFrustumRadius(750.f);

	//pObject->Collider3D()->SetScale(Vec3(1.f, 1.f, 1.f));

	//Ptr<CTexture> pColor = CAssetMgr::GetInst()->FindAsset<CTexture>(
	//	L"Texture\\HeightMap\\MoonCrater.png");
	//pObject->GetRenderComponent()->GetMaterial(0)->SetTexParam(TEX_0, pColor);

	//Ptr<CTexture> pNormal = CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\LandScapeTexture\\gl1_ground_II_normal.TGA");
	//pObject->GetRenderComponent()->GetMaterial(0)->SetTexParam(TEX_1, pNormal);
	//pLevel->AddObject(0, pObject, false);

	// 1) Prefab 로드
	Ptr<CPrefab> PoolPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\TestBullet.pref", L"Prefab\\TestBullet.pref");

	CObjectPoolMgr::GetInst()->Preload(PoolPrefab->GetProtoObject()->GetName(), 5);


	// 적 테스트

	Ptr<CMeshData> pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Testasset.fbx");

	pObject = pMeshData->Instantiate();
	pObject->SetName(L"TestTarget");
	pObject->AddComponent(new CCollider3D);
	pObject->AddComponent(new TestCharacter);

	pObject->Transform()->SetRelativePos(Vec3(3000.f, 0.f, 1000.f));
	pObject->Transform()->SetRelativeScale(Vec3(5.f, 5.f, 5.f));
	pObject->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	pObject->Transform()->SetFrustumRadius(750.f);

	pObject->Collider3D()->SetScale(Vec3(200.f, 200.f, 200.f));

	pLevel->AddObject(7, pObject, false);

	// 파티클 테스트
	//CGameObject* pTestObj = nullptr;
	//pTestObj = new CGameObject;

	//pTestObj->SetName(L"Test Object");
	//pTestObj->AddComponent(new CParticleSystem);
	//pTestObj->AddComponent(new ParticleController);

	//pTestObj->ParticleSystem()->SetParticleTexture(CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\particle\\smokeparticle.png", L"Texture\\particle\\smokeparticle.png"));
	//pTestObj->Transform()->SetRelativePos(Vec3(350.f, 100.f, 350.f));
	////pObject->Transform()->SetRelativeScale(Vec3(5.f, 5.f, 5.f));
	////pObject->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	////pObject->Transform()->SetFrustumRadius(750.f);

	//pLevel->AddObject(0, pTestObj, false);

	//pTestObj = new CGameObject;

	//pTestObj->SetName(L"Test Object2");
	//pTestObj->AddComponent(new CParticleSystem);
	//pTestObj->AddComponent(new ParticleController);

	//pTestObj->ParticleSystem()->SetParticleTexture(CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\particle\\TX_HitFlash02.png", L"Texture\\particle\\TX_HitFlash02.png"));
	//pTestObj->Transform()->SetRelativePos(Vec3(350.f, 100.f, 350.f));
	//pLevel->AddObject(0, pTestObj, false);


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

	pObject->AddChild(pVision);

	


	// Door
	CGameObject* pDoorObj = CAssetMgr::GetInst()->LoadFBX(L"FBX\\door.fbx")->Instantiate();
	pDoorObj->SetName(L"Door");
	pDoorObj->Transform()->SetRelativePos(Vec3(2000.f, -500.f, 1500.f));
	pDoorObj->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1000.f));
	pDoorObj->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	
	pDoorObj->AddComponent(new CCollider3D);
	pDoorObj->Collider3D()->SetScale(Vec3(800.f, 2000.f, 40.f));
	pDoorObj->Collider3D()->SetOffset(Vec3(400.f, 1000.f, 0.f));
	pDoorObj->Collider3D()->SetIndependentScale(true);
	
	pDoorObj->AddComponent(new DoorScript);
	
	pLevel->AddObject(1, pDoorObj, false);
}
