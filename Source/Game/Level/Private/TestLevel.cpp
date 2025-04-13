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
#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"
#include "Game/Gameplay/Weapon/Public/ThrowableController.h"
#include "Game/Gameplay/TestSound.h"

#include "Game/GamePlay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Inventory/Public/Item.h"

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
	pLevel->GetLayer(1)->SetName(L"Tile");
	pLevel->GetLayer(2)->SetName(L"Default");
	pLevel->GetLayer(3)->SetName(L"PlayerTPS");
	pLevel->GetLayer(4)->SetName(L"PlayerFPS");
	pLevel->GetLayer(5)->SetName(L"PlayerObject");
	pLevel->GetLayer(6)->SetName(L"Item");
	pLevel->GetLayer(7)->SetName(L"MonsterObject");
	pLevel->GetLayer(8)->SetName(L"UI");

	// 충돌 설정
	CCollisionMgr::GetInst()->CollisionCheck(0, 0);
	CCollisionMgr::GetInst()->CollisionCheck(3, 0);
	CCollisionMgr::GetInst()->CollisionCheck(3, 6);

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

	// ======
	// Player
	// ======
	//pObject = new CGameObject;
	//pObject->SetName(L"TestPlayer");
	//pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CCollider3D);

	//pObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
	//pObject->MeshRender()->SetMaterial(
	//	CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	//pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
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


	// ==============
	// UI System Test
	// ==============
	
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

	// CanvasUI
	CGameObject* CanvasUI = new CGameObject;
	CanvasUI->SetName(L"CanvasUI");
	CanvasUI->AddComponent(new CUI(UI_CANVAS));

	CanvasUI->AddComponent(new CUIRender);
	CanvasUI->UIRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	CanvasUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
	CanvasUI->UI()->SetColor(Vec4(0.f, 0.f, 0.f, 0.3f));
	CanvasUI->UI()->SetPriority(0);
	CanvasUI->UI()->SetRectPos(0.f, 0.f);
	CanvasUI->UI()->SetRectSize(1280.f, 768.f);

	pLevel->AddObject(8, CanvasUI, false);	// UI layer

	// ButtonUI
	CGameObject* UI = new CGameObject;
	UI->SetName(L"ButtonUI");
	UI->AddComponent(new CUI(UI_BUTTON));

	UI->AddComponent(new CUIRender);
	UI->UIRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	UI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
	UI->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	UI->UI()->SetRectPos(-250.f, 120.f);
	UI->UI()->SetRectSize(100.f, 40.f);
	UI->UI()->AddText(L"Click", 0.f, 0.f, 16, FONT_RGBA(255, 20, 20, 255));

	CanvasUI->AddChild(UI);

	// DropUI
	UI = new CGameObject;
	UI->SetName(L"DropUI");
	UI->AddComponent(new CUI(UI_DROP));

	UI->AddComponent(new CUIRender);
	UI->UIRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	UI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
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
	UI = UI->Clone();
	UI->UI()->SetRectSize(160.f, 600.f);
	UI->UI()->SetRectPos(-200.f, 0.f);
	CanvasUI->AddChild(UI);

	// 주변
	UI = UI->Clone();
	UI->UI()->SetRectSize(160.f, 600.f);
	UI->UI()->SetRectPos(-440.f, 0.f);
	CanvasUI->AddChild(UI);


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

			pObj->AddComponent(new PlayerCharacter);
			pObj->Collider3D()->SetScale(Vec3(1000.f, 1000.f, 1000.f));
			pObj->Collider3D()->SetIndependentScale(true);

			pObj->Animator3D()->SetClipTime(0, 0.3f * i);

			// 자식 메쉬들 같이 이동
			pLevel->AddObject(3, pObj, true);

			// Inventory Object
			CGameObject* pInventory = new CGameObject;
			pInventory->SetName(L"Inventory");
			pInventory->AddComponent(new CCollider3D);

			pInventory->Collider3D()->SetScale(Vec3(2000.f, 2000.f, 2000.f));
			pInventory->Collider3D()->SetIndependentScale(true);
			pInventory->Collider3D()->SetOffset(Vec3(0.f, 1000.f, 0.f));

			pInventory->AddComponent(new InventoryController);
			InventoryController* pInvenScript = static_cast<InventoryController*>(pInventory->GetScript(INVENTORYSCRIPT));

			for (int i = 0; i < 10; ++i)
			{
				// DragUI
				CGameObject* DragUI = new CGameObject;
				DragUI->SetName(L"ItemUI");
				DragUI->AddComponent(new CUI(UI_DRAG));

				DragUI->AddComponent(new CUIRender);
				DragUI->UIRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
				DragUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
				DragUI->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
				DragUI->UI()->SetRectPos(0.f, 200.f - 43.f * i);
				DragUI->UI()->SetRectSize(150.f, 40.f);

				DragUI->UI()->ClearText();
				DragUI->UI()->AddText(L"Item Name", 45.f, 12.f, 12, FONT_RGBA(255, 255, 255, 255));
				DragUI->UI()->AddText(L"20", 130.f, 12.f, 12, FONT_RGBA(255, 255, 255, 255));

				//DragUI->SetActive(false);
				UI->AddChild(DragUI);

				CGameObject* ChildUI = new CGameObject;
				ChildUI->SetName(L"ItemImageUI");
				ChildUI->AddComponent(new CUI);
				ChildUI->AddComponent(new CUIRender);
				ChildUI->UIRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
				ChildUI->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
				ChildUI->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
				ChildUI->UI()->SetRectPos(-55.f, 0.f);
				ChildUI->UI()->SetRectSize(40.f, 40.f);
				ChildUI->UI()->SetImage(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\Idle_Left.bmp"));

				DragUI->AddChild(ChildUI);

				// inventory controller에 ui 등록
				pInvenScript->AddVicinityUI(DragUI);
			}

			pInvenScript->SetPlayer(pObj);
			pObj->AddChild(pInventory);

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


			pWeaponObj->AddComponent(new ItemScript);
			ItemScript* pItem = static_cast<ItemScript*>(pWeaponObj->GetScript(ITEMSCRIPT));
			pItem->SetCount(1);
			pItem->SetItemType(ITEM_TYPE::WEAPON);
			pItem->SetImage(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\UI\\AKM.png"));

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

			pWeaponObj->AddComponent(new ItemScript);
			pItem = static_cast<ItemScript*>(pWeaponObj->GetScript(ITEMSCRIPT));
			pItem->SetCount(1);
			pItem->SetItemType(ITEM_TYPE::GRENADE);
			pItem->SetImage(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\UI\\Grenade.png"));

			pLevel->AddObject(6, pWeaponObj, false);

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

			pWeaponObj->AddComponent(new ItemScript);
			pItem = static_cast<ItemScript*>(pWeaponObj->GetScript(ITEMSCRIPT));
			pItem->SetCount(1);
			pItem->SetItemType(ITEM_TYPE::SMOKEBOMB);
			pItem->SetImage(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\UI\\SmokeBomb.png"));

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

			pWeaponObj->AddComponent(new ItemScript);
			pItem = static_cast<ItemScript*>(pWeaponObj->GetScript(ITEMSCRIPT));
			pItem->SetCount(1);
			pItem->SetItemType(ITEM_TYPE::ENERGY_DRINK);
			pItem->SetImage(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\UI\\EnergyDrink.png"));

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

			pWeaponObj->AddComponent(new ItemScript);
			pItem = static_cast<ItemScript*>(pWeaponObj->GetScript(ITEMSCRIPT));
			pItem->SetCount(1);
			pItem->SetItemType(ITEM_TYPE::FIRST_AID_KIT);
			pItem->SetImage(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\UI\\FirstAidKit.png"));

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

		pObj->AddComponent(new ItemScript);
		ItemScript* pItem = static_cast<ItemScript*>(pObj->GetScript(ITEMSCRIPT));
		pItem->SetCount(1);
		pItem->SetItemType(ITEM_TYPE::ADRENALINE_SYRINGE);
		pItem->SetImage(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\UI\\AdrenalineSyringe.png"));

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

		pObj->AddComponent(new ItemScript);
		pItem = static_cast<ItemScript*>(pObj->GetScript(ITEMSCRIPT));
		pItem->SetCount(1);
		pItem->SetItemType(ITEM_TYPE::BANDAGE);
		pItem->SetImage(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\UI\\Bandage.png"));

		pLevel->AddObject(6, pObj, false);

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
		pLevel->AddObject(0, pObj, false);




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
}
