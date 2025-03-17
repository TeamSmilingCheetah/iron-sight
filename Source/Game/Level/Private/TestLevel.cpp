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
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CCollisionMgr.h"
#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

void TestLevel::CreateTestLevel()
{
	// Texture 로딩하기
	Ptr<CTexture> pTex = CAssetMgr::GetInst()->Load<CTexture>(
		L"PlayerTex", L"Texture\\Character.png");
	Ptr<CTexture> pAtlasTex = CAssetMgr::GetInst()->Load<CTexture>(
		L"TileMapTex", L"Texture\\TILE.bmp");

	auto pLevel = new CLevel;

	// 테스트 레벨을 현재 레벨로 지정
	ChangeLevel(pLevel, LEVEL_STATE::STOP);

	pLevel->GetLayer(0)->SetName(L"Background");
	pLevel->GetLayer(1)->SetName(L"Tile");
	pLevel->GetLayer(2)->SetName(L"Default");
	pLevel->GetLayer(3)->SetName(L"Player");
	pLevel->GetLayer(4)->SetName(L"PlayerObject");
	pLevel->GetLayer(5)->SetName(L"Monster");
	pLevel->GetLayer(6)->SetName(L"MonsterObject");

	// 충돌 설정
	CCollisionMgr::GetInst()->CollisionCheck(0, 0);

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
		L"Texture\\skybox\\Sky02.jpg");

	// FrustumCheck 비활성화
	pSkyBox->Transform()->SetFrustumCheck(false);

	pSkyBox->SkyBox()->SetMode(SPHERE);
	pSkyBox->SkyBox()->SetSkyBoxTexture(pSkyBoxTex);
	pLevel->AddObject(0, pSkyBox, false);

	// ======
	// Player
	// ======
	pObject = new CGameObject;
	pObject->SetName(L"Player");
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider3D);

	pObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetMaterial(
		CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
	pObject->Transform()->SetRelativeScale(Vec3(500.f, 500.f, 500.f));
	pObject->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	pObject->Transform()->SetFrustumRadius(750.f);

	pObject->Collider3D()->SetScale(Vec3(1.f, 1.f, 1.f));

	Ptr<CTexture> pColor = CAssetMgr::GetInst()->FindAsset<CTexture>(
		L"Texture\\HeightMap\\MoonCrater.png");
	pObject->GetRenderComponent()->GetMaterial(0)->SetTexParam(TEX_0, pColor);

	//Ptr<CTexture> pNormal = CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\LandScapeTexture\\gl1_ground_II_normal.TGA");
	//pObject->GetRenderComponent()->GetMaterial(0)->SetTexParam(TEX_1, pNormal);
	pLevel->AddObject(0, pObject, false);

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

	// ============
	// FBX Loading
	// ============
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;


		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\pubg_test2.fbx");
		//pMeshData = CAssetMgr::GetInst()->FindAsset<CMeshData>(L"MeshData\\Monster.mdat");

		Ptr<CMeshData> pWeaponModel = CAssetMgr::GetInst()->LoadFBX(L"FBX\\ak47_test.fbx");

		int modelCnt = 1;
		for (int i = 0; i < modelCnt; ++i)
		{
			pObj = pMeshData->Instantiate();
			pObj->SetName(L"Character");
			pObj->AddComponent(new CCollider3D);
			pObj->AddComponent(new CColliderRay);

			pObj->Transform()->SetRelativePos(Vec3(500.f + i * 200.f, -380.f, 500.f));
			pObj->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));
			pObj->Transform()->SetRelativeRotation(0.f, 90.f, 0.f);

			pObj->ColliderRay()->SetRayDir(Vec3(0.f, 0.f, -1.f));

			pObj->AddComponent(new PlayerCharacter);
			pObj->Collider3D()->SetScale(Vec3(1000.f, 1000.f, 1000.f));
			pObj->Collider3D()->SetIndependentScale(true);

			pObj->Animator3D()->SetClipTime(0, 0.3f * i);

			pLevel->AddObject(0, pObj, false);

			CGameObject* pWeaponObj = pWeaponModel->Instantiate();
			pWeaponObj->SetName(L"Weapon");
			pWeaponObj->AddComponent(new CColliderRay);
			
			pWeaponObj->Transform()->SetRelativePos(Vec3(90.f, 0.f, 30.f));
			pWeaponObj->Transform()->SetRelativeScale(Vec3(900.f, 900.f, 900.f));
			pWeaponObj->Transform()->SetRelativeRotation(0.f, -2.f, -4.3f);
			pWeaponObj->Transform()->SetIndependentScale(true);

			pWeaponObj->ColliderRay()->SetRayPos(Vec3(-30.f, 100.f, 0.f));
			pWeaponObj->ColliderRay()->SetRayDir(Vec3(1.f, 0.f, 0.f));
			
			pObj->AddChild(pWeaponObj);
		}


		// 모바일 배그 애셋 테스트
		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\DROP.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Airdrop");
		pObj->Transform()->SetRelativeScale(Vec3(15.f, 15.f, 15.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Adrenaline Syringe.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Adrenaline");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Bandage.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Bandage");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\box.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"DeathBox");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Energy Drink.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Energy Drink");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\First Aid Kit.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"First Aid Kit");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\GRENADE.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Grenade");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Med Kit.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Med Kit");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Molotov.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Molotov");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Painkiller.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Painkiller");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);

		pMeshData = CAssetMgr::GetInst()->LoadFBX(L"FBX\\M18_Smoke.fbx");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Smoke Grenade");
		pObj->Transform()->SetRelativeScale(Vec3(150.f, 150.f, 150.f));
		pObj->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);
		pLevel->AddObject(0, pObj, false);
	}
}
