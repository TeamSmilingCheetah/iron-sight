#include "pch.h"
#include "Game/Factory/Public/GameFactory.h"

#include "Engine/API/Public/EngineFacade.h"
#include "Client/API/Public/ClientFacade.h"

// TODO(KHJ): 해당 헤더들과의 결합성 배제할 방법 찾기
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Light/CLight3D.h"

#include "Game/Gameplay/Character/Public/CameraController.h"

// XXX(KHJ): namespace 사용해서 코드 작성을 편하게 할지, 가독성을 좋게 할지 고민해야 할 부분
// using namespace Engine;

/** Creation **/

class CameraController;

unique_ptr<CGameObject> GameFactory::CreateObject()
{
	unique_ptr<CGameObject> ObjectPtr = Engine::Common::CreateNewObject();
	return std::move(ObjectPtr);
}

unique_ptr<CLevel> GameFactory::CreateLevel()
{
	unique_ptr<CLevel> LevelPtr = Engine::Level::CreateNewLevel();
	return std::move(LevelPtr);
}

/** Set Up Level **/

/**
 * @brief 기본적인 Layer 정의
 */
void GameFactory::LoadDefaultLayer(CLevel* PLevel)
{
	Engine::Layer::SetLayerName(PLevel, 0, L"Background");
	Engine::Layer::SetLayerName(PLevel, 1, L"Structure");
	Engine::Layer::SetLayerName(PLevel, 2, L"Default");
	Engine::Layer::SetLayerName(PLevel, 3, L"PlayerTPS");
	Engine::Layer::SetLayerName(PLevel, 4, L"PlayerFPS");
	Engine::Layer::SetLayerName(PLevel, 5, L"PlayerObject");
	Engine::Layer::SetLayerName(PLevel, 6, L"Item");
	Engine::Layer::SetLayerName(PLevel, 7, L"MonsterObject");
	Engine::Layer::SetLayerName(PLevel, 8, L"UI");
	Engine::Layer::SetLayerName(PLevel, 9, L"ObjectPool");
	Engine::Layer::SetLayerName(PLevel, 10, L"BulletLayer");
}

/**
 * @brief Collision Manager에서 세팅하는 기본적인 레이어 간의 충돌 정의
 */
void GameFactory::LoadDefaultCollisionSetting()
{
	// TODO(KHJ): 한 레이어가 Args 다수에 대해 지정하는 형태로 API 구축하기
	// Toggle 대신 Default가 충돌 안함 상태, Setting 하면 충돌하는 것으로 변경되도록 한 함수가 하나의 기능만 맡도록 변경하기
	Engine::Layer::SetLayerCollision(0, 0);
	Engine::Layer::SetLayerCollision(0, 1);
	Engine::Layer::SetLayerCollision(0, 7);
	Engine::Layer::SetLayerCollision(3, 0);
	Engine::Layer::SetLayerCollision(3, 1);
	Engine::Layer::SetLayerCollision(3, 6);
	Engine::Layer::SetLayerCollision(3, 7);

	// 총알이 충돌될 레이어
	Engine::Layer::SetLayerCollision(0, 10);
	Engine::Layer::SetLayerCollision(1, 10);
	Engine::Layer::SetLayerCollision(3, 10);
	Engine::Layer::SetLayerCollision(4, 10);
	Engine::Layer::SetLayerCollision(7, 10);
}

/**
 * @brief
 * @param PLevel 해당 카메라를 로드할 레벨
 */
void GameFactory::LoadMainCamera(CLevel* PLevel)
{
	auto Camera = Engine::Common::CreateNewObject();
	Engine::Common::SetObjectName(Camera.get(), L"MainCamera");

	// XXX(KHJ): 일단 Raw로, UniquePtr 고려해볼 것
	auto RawCameraPtr = Camera.get();
	Camera.release();

	Engine::Common::AddScriptToObject<CameraController>(RawCameraPtr);

	Engine::Common::AddComponentToObject<CCamera>(RawCameraPtr);
	Engine::Camera::SetCameraOptions(RawCameraPtr, PERSPECTIVE, 0);

	Engine::Common::AddComponentToObject<CColliderRay>(RawCameraPtr);
	Engine::Collider::SetColliderRayOptions(RawCameraPtr, {0, 0, 0}, 2000.f, true);

	Engine::Level::AddObjectToLayer(PLevel, RawCameraPtr, 0, false);
}

/**
 * @brief
 * @param PLevel 해당 조명을 로드할 레벨
 */
void GameFactory::LoadDefaultLight(CLevel* PLevel)
{
	auto PointLight = Engine::Common::CreateNewObject();

	// XXX(KHJ): 일단 Raw로, UniquePtr 고려해볼 것
	auto RawPointLightPtr = PointLight.get();
	PointLight.release();

	Engine::Common::SetObjectName(RawPointLightPtr, L"PointLight");

	Engine::Common::AddComponentToObject<CLight3D>(RawPointLightPtr);
	Engine::Light::Set3DLightProperties(RawPointLightPtr, LIGHT_TYPE::DIRECTIONAL,
	                                    {1.f, 1.f, 1.f}, {0.15f, 0.15f, 0.15f}, 0.3f, 300.f);

	Engine::Transform::SetPositionAndRotation(RawPointLightPtr, {0.f, -450.f, 0.f}, {45.f, 45.f, 0.f});

	Engine::Level::AddObjectToLayer(PLevel, RawPointLightPtr, 0, false);
}

/**
 * @brief
 * @param PLevel 해당 SkyBox를 로드할 레벨
 */
void GameFactory::LoadDefaultSkyBox(CLevel* PLevel)
{
	auto SkyBox = Engine::Common::CreateNewObject();

	// XXX(KHJ): 일단 Raw로, UniquePtr 고려해볼 것
	auto RawSkyBoxPtr = SkyBox.get();
	SkyBox.release();

	Engine::Common::SetObjectName(RawSkyBoxPtr, L"SkyBox");

	Engine::Common::AddComponentToObject<CSkyBox>(RawSkyBoxPtr);
	auto SkyBoxTexture = Engine::IO::LoadAsset<CTexture>(L"Texture\\skybox\\Sky01.png");
	Engine::SkyBox::SetSkyBoxProperties(RawSkyBoxPtr, SKYBOX_MODE::SPHERE, SkyBoxTexture);

	// FrustumCheck 비활성화
	Engine::Transform::SetFrustumCheck(RawSkyBoxPtr, false);

	Engine::Level::AddObjectToLayer(PLevel, RawSkyBoxPtr, 0, false);
}

/**
 *
 * @param PLevel 해당 Landscape를 로드할 레벨
 */
void GameFactory::LoadDefaultLandscape(CLevel* PLevel)
{
	auto Landscape = Engine::Common::CreateNewObject();

	// XXX(KHJ): 일단 Raw로, UniquePtr 고려해볼 것
	auto RawLandscapePtr = Landscape.get();
	Landscape.release();

	Engine::Common::SetObjectName(RawLandscapePtr, L"LandScape");

	Engine::Common::AddComponentToObject<CLandScape>(RawLandscapePtr);
	Engine::Transform::SetPositionAndRotation(RawLandscapePtr, {0.f, -500.f, 0.f}, {500.f, 500.f, 500.f});

	auto ColorTexture = Engine::IO::LoadAsset<CTexture>(L"Texture\\LandScapeTexture\\LS_Color.dds");
	auto NormalTexture = Engine::IO::LoadAsset<CTexture>(L"Texture\\LandScapeTexture\\LS_Normal.dds");

	Engine::Landscape::SetLandscapeProperties(RawLandscapePtr, {32, 32}, {1024, 1024}, ColorTexture, NormalTexture);

	Engine::Level::AddObjectToLayer(PLevel, RawLandscapePtr, 0, false);
}

CGameObject* GameFactory::MakeFBXObject(
	CLevel* PLevel,
	const wstring& PFilePath,
	const wstring& PName,
	const Vec3& PPosition,
	const Vec3& PRotation,
	const Vec3& PScale,
	const vector<function<void(CGameObject*)>>& PComponentSetups,
	int PLayer,
	bool PMoveWithChild
)
{
	// FBX Load & Instantiate
	Ptr<CMeshData> MeshData = Engine::IO::LoadFBX(PFilePath);
	auto FBXObject = Engine::Common::InstantiateNewObject(MeshData);

	Engine::Common::SetObjectName(FBXObject, PName);

	// Set Transform
	Engine::Transform::SetPositionAndRotation(FBXObject, PPosition, PRotation);
	Engine::Transform::SetScale(FBXObject, PScale);

	// Set Components
	for (const auto& Setup : PComponentSetups)
	{
		Setup(FBXObject);
	}

	// Add Object
	Engine::Level::AddObjectToLayer(PLevel, FBXObject, PLayer, PMoveWithChild);

	return FBXObject;
}

void GameFactory::MakeCloneObject(CLevel* PLevel, CGameObject* POrigin, bool PMoveWithChild, int PCopyMount)
{
	for (int i = 0; i <= PCopyMount; i++)
	{
		Engine::Level::AddObjectToLayer(PLevel, POrigin->Clone(), POrigin->GetLayerIdx(), PMoveWithChild);
	}
}
