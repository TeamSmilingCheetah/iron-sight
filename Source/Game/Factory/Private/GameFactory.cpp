#include "pch.h"
#include "Game/Factory/Public/GameFactory.h"

#include "Engine/API/Public/EngineFacade.h"
#include "Client/API/Public/ClientFacade.h"

// TODO(KHJ): 해당 헤더들과의 결합성 배제할 방법 찾기
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Light/CLight3D.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"
#include "Engine/Runtime/Public/Component/Physics/RayCollider.h"
#include "Engine/Runtime/Public/Component/Physics/SphereCollider.h"

#include "Engine/System/Public/Manager/CStateMgr.h"


using namespace Engine;

/** Creation **/

class CameraController;

unique_ptr<CGameObject> GameFactory::CreateObject()
{
	unique_ptr<CGameObject> ObjectPtr = Common::CreateNewObject();
	return std::move(ObjectPtr);
}

unique_ptr<CLevel> GameFactory::CreateLevel()
{
	unique_ptr<CLevel> LevelPtr = Level::CreateNewLevel();
	return std::move(LevelPtr);
}

/** Set Up Level **/

/**
 * @brief 기본적인 Layer 정의
 */
void GameFactory::LoadDefaultLayer(CLevel* PLevel)
{
	Layer::SetLayerName(PLevel, 0, L"Background");
	Layer::SetLayerName(PLevel, 1, L"Structure");
	Layer::SetLayerName(PLevel, 2, L"Default");
	Layer::SetLayerName(PLevel, 3, L"PlayerTPS");
	Layer::SetLayerName(PLevel, 4, L"PlayerFPS");
	Layer::SetLayerName(PLevel, 5, L"PlayerObject");
	Layer::SetLayerName(PLevel, 6, L"Item");
	Layer::SetLayerName(PLevel, 7, L"MonsterObject");
	Layer::SetLayerName(PLevel, 8, L"UI");
	Layer::SetLayerName(PLevel, 9, L"ObjectPool");
	Layer::SetLayerName(PLevel, 10, L"BulletLayer");
	Layer::SetLayerName(PLevel, 20, L"PostProcessLayer");
}

/**
 * @brief Collision Manager에서 세팅하는 기본적인 레이어 간의 충돌 정의
 */
void GameFactory::LoadDefaultCollisionSetting()
{
	// TODO(KHJ): 한 레이어가 Args 다수에 대해 지정하는 형태로 API 구축하기
	// Toggle 대신 Default가 충돌 안함 상태, Setting 하면 충돌하는 것으로 변경되도록 한 함수가 하나의 기능만 맡도록 변경하기
	Layer::SetLayerCollision(0, 0);
	Layer::SetLayerCollision(0, 1);
	Layer::SetLayerCollision(0, 5);
	Layer::SetLayerCollision(0, 7);
	Layer::SetLayerCollision(3, 0);
	Layer::SetLayerCollision(3, 1);
	Layer::SetLayerCollision(3, 6);
	Layer::SetLayerCollision(3, 7);

	Layer::SetLayerCollision(7, 0);
	Layer::SetLayerCollision(7, 1);

	// 총알이 충돌될 레이어
	Layer::SetLayerCollision(0, 10);
	Layer::SetLayerCollision(1, 10);
	Layer::SetLayerCollision(3, 10);
	Layer::SetLayerCollision(4, 10);
	Layer::SetLayerCollision(7, 10);
}

/**
 * @brief
 * @param PLevel 해당 카메라를 로드할 레벨
 */
void GameFactory::LoadMainCamera(CLevel* PLevel)
{
	auto Camera = Common::CreateNewObject();
	Common::SetObjectName(Camera.get(), L"MainCamera");

	// XXX(KHJ): 일단 Raw로, UniquePtr 고려해볼 것
	auto RawCameraPtr = Camera.get();
	Camera.release();

	Common::AddScriptToObject(RawCameraPtr, SCRIPT_TYPE::CAMERASCRIPT);

	Common::AddComponentToObject<CCamera>(RawCameraPtr);
	Camera::SetCameraOptions(RawCameraPtr, PERSPECTIVE, 0);

	Common::AddComponentToObject<FRayCollider>(RawCameraPtr);
	Collider::SetRayColliderProperties(RawCameraPtr, {0, 0, 0}, 2000.f, true);


	Level::AddObjectToLayer(PLevel, RawCameraPtr, 0, false);
}

/**
 * @brief
 * @param PLevel 해당 조명을 로드할 레벨
 */
void GameFactory::LoadDefaultLight(CLevel* PLevel)
{
	auto PointLight = Common::CreateNewObject();

	// XXX(KHJ): 일단 Raw로, UniquePtr 고려해볼 것
	auto RawPointLightPtr = PointLight.get();
	PointLight.release();

	Common::SetObjectName(RawPointLightPtr, L"PointLight");

	Common::AddComponentToObject<CLight3D>(RawPointLightPtr);
	Light::Set3DLightProperties(RawPointLightPtr, LIGHT_TYPE::DIRECTIONAL,
	                            {1.f, 1.f, 1.f}, {0.15f, 0.15f, 0.15f}, 0.3f, 300.f);

	Transform::SetPositionAndRotation(RawPointLightPtr, {0.f, -450.f, 0.f}, {45.f, 45.f, 0.f});

	Level::AddObjectToLayer(PLevel, RawPointLightPtr, 0, false);
}

/**
 * @brief
 * @param PLevel 해당 SkyBox를 로드할 레벨
 */
void GameFactory::LoadDefaultSkyBox(CLevel* PLevel)
{
	auto SkyBox = Common::CreateNewObject();

	// XXX(KHJ): 일단 Raw로, UniquePtr 고려해볼 것
	auto RawSkyBoxPtr = SkyBox.get();
	SkyBox.release();

	Common::SetObjectName(RawSkyBoxPtr, L"SkyBox");

	Common::AddComponentToObject<CSkyBox>(RawSkyBoxPtr);
	auto SkyBoxTexture = IO::LoadAsset<CTexture>(L"Texture\\skybox\\Sky01.png");
	SkyBox::SetSkyBoxProperties(RawSkyBoxPtr, SKYBOX_MODE::SPHERE, SkyBoxTexture);

	// FrustumCheck 비활성화
	Transform::SetFrustumCheck(RawSkyBoxPtr, false);

	Level::AddObjectToLayer(PLevel, RawSkyBoxPtr, 0, false);
}

/**
 *
 * @param PLevel 해당 Landscape를 로드할 레벨
 */
void GameFactory::LoadDefaultLandscape(CLevel* PLevel)
{
	auto Landscape = Common::CreateNewObject();

	// XXX(KHJ): 일단 Raw로, UniquePtr 고려해볼 것
	auto RawLandscapePtr = Landscape.get();
	Landscape.release();

	Common::SetObjectName(RawLandscapePtr, L"LandScape");

	Common::AddComponentToObject<FLandscape>(RawLandscapePtr);
	Transform::SetPosition(RawLandscapePtr, {0.f, -500.f, 0.f});
	Transform::SetScale(RawLandscapePtr, {5000.f, 5000.f, 5000.f});
	Transform::SetFrustumCheck(RawLandscapePtr, false);

	auto ColorTexture = IO::LoadAsset<CTexture>(L"Texture\\LandScapeTexture\\LS_Color.dds");
	auto NormalTexture = IO::LoadAsset<CTexture>(L"Texture\\LandScapeTexture\\LS_Normal.dds");

	Landscape::SetLandscapeProperties(RawLandscapePtr, {32, 32}, {1024, 1024}, ColorTexture, NormalTexture);

	Level::AddObjectToLayer(PLevel, RawLandscapePtr, 0, false);
}

CGameObject* GameFactory::LoadDefaultPlayer(CLevel* PLevel, const Vec3& PPosition)
{
	// Load Animation & MeshData From FBX
	Ptr<CMeshData> PlayerMeshData = IO::LoadFBX(L"FBX\\Character\\James.fbx");

	CGameObject* Player = Common::InstantiateNewObject(PlayerMeshData);
	Common::SetObjectName(Player, L"Player");

	Transform::SetPositionAndRotation(Player, PPosition, {0.f, 90.f, 0.f});
	Transform::SetScale(Player, {6.f, 6.f, 6.f});

	Common::AddComponentToObject<CStateMachine>(Player);

	// StateMachine
	StateMachine::AddState(Player, Common::LoadState(L"Player_Idle"));
	StateMachine::AddState(Player, Common::LoadState(L"Player_Jump_Up"));
	StateMachine::AddState(Player, Common::LoadState(L"Player_Jump_Loop"));
	StateMachine::AddState(Player, Common::LoadState(L"Player_Jump_Down"));
	StateMachine::AddState(Player, Common::LoadState(L"Player_Dead"));
	StateMachine::AddState(Player, Common::LoadState(L"Player_Heal"));
	StateMachine::AddState(Player, Common::LoadState(L"Player_Grenade_Prepare"));
	StateMachine::AddState(Player, Common::LoadState(L"Player_Grenade_Throw_Low"));
	StateMachine::AddState(Player, Common::LoadState(L"Player_Grenade_Throw_High"));
	StateMachine::AddState(Player, Common::LoadState(L"Player_Gun_Reload"));
	StateMachine::AddState(Player, Common::LoadState(L"Player_Gun_Fire"));

	StateMachine::SetDefaultState(Player, L"Player_Idle");

	StateMachine::AddAnyTransition(Player, L"Player_Idle");
	StateMachine::AddAnyTransition(Player, L"Player_Dead");
	StateMachine::AddAnyTransition(Player, L"Player_Jump_Loop"); // 떨어질 때

	StateMachine::AddTransition(Player, L"Player_Idle", L"Player_Jump_Up");		// 점프 시작
	StateMachine::AddTransition(Player, L"Player_Jump_Loop", L"Player_Jump_Down");	// 착지
	StateMachine::AddTransition(Player, L"Player_Idle", L"Player_Heal");
	StateMachine::AddTransition(Player, L"Player_Idle", L"Player_Gun_Fire");
	StateMachine::AddTransition(Player, L"Player_Idle", L"Player_Gun_Reload");
	StateMachine::AddTransition(Player, L"Player_Idle", L"Player_Grenade_Prepare");

	StateMachine::AddTransition(Player, L"Player_Gun_Fire", L"Player_Gun_Reload");

	StateMachine::AddTransition(Player, L"Player_Grenade_Prepare", L"Player_Grenade_Throw_Low");
	StateMachine::AddTransition(Player, L"Player_Grenade_Prepare", L"Player_Grenade_Throw_High");

	Common::AddComponentToObject<FRayCollider>(Player);
	Collider::SetRayColliderProperties(Player, {0.f, 0.f, -1.f}, {0.f, 970.f, 0.f}, 5000.f, true);

	// Animation
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_rifle_jump.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_firing_rifle.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_firing_rifle.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_reloading.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_reloading.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_death_from_the_front.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_death.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_walk_crouching_forward.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_forward.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_run_forward.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_walk_forward.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_walk_crouching_left.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_left.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_run_left.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_walk_left.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_walk_crouching_backward.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_backward.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_run_backward.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_walk_backward.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_walk_crouching_right.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_right.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_run_right.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_walk_right.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_idle.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_idle_crouching.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_idle.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_jump_up.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_jump_loop.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_jump_down.anim"));

	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_first_aid_kit.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_energy_drink.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_adrenaline.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_pain_killer.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_stand_first_aid_kit.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_stand_energy_drink.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_stand_adrenaline.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_stand_pain_killer.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_toss_grenade_low.anim"));
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_stand_grenade_prepare.anim"));
	// TEST(Ssio): PUBG_ANIMSET에서 자름
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_toss_grenade_test.anim"));
	// TEST(Ssio): toss_grenade 뒷부분 자름
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_grenade_prepare.anim"));
	// TEST(Ssio): prone_toss_grenade 앞부분 자름
	Animation::AddAnimationClip(Player, IO::LoadAsset<CAnimation>(L"Animation\\Armature_prone_toss_grenade_test.anim"));
	// TEST(Ssio): prone_toss_grenade 뒷부분 자름

	Common::AddComponentToObject<FBoxCollider>(Player);
	Collider::SetColliderProperties(Player, {300.f, 910.f, 300.f}, {0.f, 455.f, 0.f}, true);
	Collider::SetColliderDynamic(Player, EColliderType::BoxCollider);

	// Player Head Collider
	auto HeaderCollider = Common::CreateNewObject();

	// XXX(KHJ): 일단 Raw로, UniquePtr 고려해볼 것
	auto RawHeaderCollider = HeaderCollider.get();
	HeaderCollider.release();

	// Object Setting
	Common::SetObjectName(RawHeaderCollider, L"Player Head");
	Common::AddComponentToObject<FBoxCollider>(RawHeaderCollider);

	Transform::SetPosition(RawHeaderCollider, {0.f, 170.f, 0.f});
	Collider::SetColliderProperties(RawHeaderCollider, {150.f, 150.f, 150.f}, {0, -27.f, 0}, true, false);

	Common::AddChild(Player, RawHeaderCollider);

	// GroundCheckRay Object
	auto GroundCheckRay = Common::CreateNewObject();
	auto RawGroundCheckRay = GroundCheckRay.get();
	GroundCheckRay.release();

	Common::SetObjectName(RawGroundCheckRay, L"Player Ground Check Ray");
	Common::AddComponentToObject<FRayCollider>(RawGroundCheckRay);
	Collider::SetRayColliderProperties(RawGroundCheckRay, Vec3(0.f, -1.f, 0.f), Vec3(0.f, 500.f, 0.f), 1500.f, true);

	Common::AddScriptToObject(RawGroundCheckRay, SCRIPT_TYPE::GROUNDCHECK);

	Common::AddChild(Player, RawGroundCheckRay);

	Level::AddObjectToLayer(PLevel, Player, 3, true);

	return Player;
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
	Ptr<CMeshData> MeshData = IO::LoadFBX(PFilePath);
	auto FBXObject = Common::InstantiateNewObject(MeshData);

	Common::SetObjectName(FBXObject, PName);

	// Set Transform
	Transform::SetPositionAndRotation(FBXObject, PPosition, PRotation);
	Transform::SetScale(FBXObject, PScale);

	// Set Components
	for (const auto& Setup : PComponentSetups)
	{
		Setup(FBXObject);
	}

	// Add Object
	Level::AddObjectToLayer(PLevel, FBXObject, PLayer, PMoveWithChild);

	return FBXObject;
}

void GameFactory::MakeCloneObject(CLevel* PLevel, CGameObject* POrigin, bool PMoveWithChild, int PCopyMount)
{
	for (int i = 0; i <= PCopyMount; i++)
	{
		Level::AddObjectToLayer(PLevel, POrigin->Clone(), POrigin->GetLayerIdx(), PMoveWithChild);
	}
}
