#include "pch.h"
#include "Game/Factory/Public/GameFactory.h"

#include "Engine/API/Public/EngineFacade.h"
#include "Client/API/Public/ClientFacade.h"

// TODO(KHJ): 해당 헤더들과의 결합성 배제할 방법 찾기
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
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
void GameFactory::SetUpLayer(CLevel* PLevel)
{
	Engine::Layer::SetLayerName(PLevel, 0,L"Background");
	Engine::Layer::SetLayerName(PLevel, 1,L"Structure");
	Engine::Layer::SetLayerName(PLevel, 2,L"Default");
	Engine::Layer::SetLayerName(PLevel, 3,L"PlayerTPS");
	Engine::Layer::SetLayerName(PLevel, 4,L"PlayerFPS");
	Engine::Layer::SetLayerName(PLevel, 5,L"PlayerObject");
	Engine::Layer::SetLayerName(PLevel, 6,L"Item");
	Engine::Layer::SetLayerName(PLevel, 7,L"MonsterObject");
	Engine::Layer::SetLayerName(PLevel, 8,L"UI");
	Engine::Layer::SetLayerName(PLevel, 9,L"ObjectPool");
	Engine::Layer::SetLayerName(PLevel, 10,L"BulletLayer");
}

/**
 * @brief Collision Manager에서 세팅하는 기본적인 레이어 간의 충돌 정의
 */
void GameFactory::SetUpCollision()
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
void GameFactory::SetUpCamera(CLevel* PLevel)
{
	auto Camera = Engine::Common::CreateNewObject();
	Engine::Common::SetObjectName(Camera.get(), L"MainCamera");

	Engine::Common::AddScriptToObject<CameraController>(Camera.get());

	Engine::Common::AddComponentToObject<CCamera>(Camera.get());
	Engine::Camera::SetCameraOptions(Camera.get(), PERSPECTIVE, 0);

	Engine::Common::AddComponentToObject<CColliderRay>(Camera.get());
	Engine::Collider::SetColliderRayOptions(Camera.get(), {0, 0, 0}, 2000.f, true);

	// XXX(KHJ): 일단 Raw로, UniquePtr 고려해볼 것
	auto RawCameraPtr = Camera.get();
	Camera.release();

	Engine::Level::AddObjectToLayer(PLevel, RawCameraPtr, 0, false);
}
