#pragma once
#include "Common/global.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Base/Component.h"
#include "Engine/Runtime/Public/Component/Rendering/CSkyBox.h"
#include "Engine/Runtime/Public/Component/Rendering/Landscape.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/Runtime/Public/State/CState.h"

#include "Engine/System/Public/Manager/CScriptMgr.h"

enum class EColliderType : UINT8;
/**
 * @brief Game Contents Develop에 필요한 Engine API를 간접호출할 수 있도록 만든 namespace
 */
namespace Engine
{
	namespace Common
	{
		unique_ptr<CGameObject> CreateNewObject();
		CGameObject* InstantiateNewObject(Ptr<CMeshData> PMeshData);
		void SetObjectName(CGameObject* PObject, const wstring& PName);
		FComponent* GetComponent(const CGameObject* PObject, COMPONENT_TYPE PComponentType);
		void AddChild(CGameObject* PObject, CGameObject* PChild);
		CState* LoadState(const wstring& PStateName);

		template <class T>
		void AddComponentToObject(CGameObject* PObject);

		void AddScriptToObject(CGameObject* PObject, SCRIPT_TYPE PScriptType);
	}

	namespace Transform
	{
		void SetPosition(CGameObject* PObject, Vec3 PPosition);
		void SetPositionAndRotation(CGameObject* PObject, Vec3 PPosition, Vec3 PRotation);
		void SetScale(CGameObject* PObject, Vec3 PScale);
		void SetFrustumCheck(CGameObject* PObject, bool PCheck);
	}

	namespace IO
	{
		Ptr<CMeshData> LoadFBX(const wstring& PRelativeFilePath);

		template <class T>
		Ptr<T> LoadAsset(const wstring& PRelativeFilePath);
	}

	namespace Layer
	{
		void SetLayerName(CLevel* PLevel, int PLayerNumber, const wstring& PLayerName);
		void SetLayerCollision(int PLayer, int POtherLayer);
	}

	namespace Level
	{
		unique_ptr<CLevel> CreateNewLevel();
		void AddObjectToLayer(CLevel* PLevel, CGameObject* PObject, int PLayerIdx, bool PMoveWithChild);
	}

	namespace Camera
	{
		void SetCameraOptions(CGameObject* PCameraObject, PROJ_TYPE PProjectionType, int PPriority);
	}

	namespace Collider
	{
		void SetColliderProperties(CGameObject* PObject, Vec3 PScale, Vec3 POffset,
		                        bool PIsIndependentScale, bool PIsTrigger = false);

		// TODO(KHJ): Common Form 구축할 것
		void SetRayColliderProperties(CGameObject* InObject, Vec3 InDirection, Vec3 InOffset,
		                           float InLength, bool InIsTrigger);
		void SetRayColliderProperties(CGameObject* InObject, Vec3 InDirection,
		                           float InLength, bool InIsIndependant);

		void SetColliderDynamic(const CGameObject* InObject, EColliderType InType);
	}

	namespace Light
	{
		void Set3DLightProperties(CGameObject* PLightObject, LIGHT_TYPE PLightType, Vec3 PLightColor, Vec3 PAmbient,
		                          float PSpecularCoefficient, float PRadius);
	}

	namespace SkyBox
	{
		void SetSkyBoxProperties(CGameObject* PSkyBox, SKYBOX_MODE PMode, Ptr<CTexture> PTexture);
	}

	namespace Landscape
	{
		void SetLandscapeProperties(CGameObject* PLandscape, UnsignedIntegerSquare PFaceSize,
		                            UnsignedIntegerSquare PHeightMapSize,
		                            Ptr<CTexture> PColorTexture, Ptr<CTexture> PNormalTexture);
	}

	namespace Animation
	{
		void SetAnimationClips(CGameObject* PObject, Ptr<CMeshData> PAnimationSet);
		void AddAnimationClips(CGameObject* PObject, Ptr<CMeshData> PAnimationSet);
		void AddAnimationClip(CGameObject* PObject, Ptr<CAnimation> PAnimation);
	}

	namespace StateMachine
	{
		void AddState(CGameObject* PObject, CState* PState);
		void AddTransition(CGameObject* PObject, const wstring& PFrom, const wstring& PTo);
		void AddAnyTransition(CGameObject* PObject, const wstring& PTo);
		void SetDefaultState(CGameObject* PObject, const wstring& PStateName);
	}
}

template <class T>
void Engine::Common::AddComponentToObject(CGameObject* PObject)
{
	static_assert(std::is_base_of_v<FComponent, T> || std::is_base_of_v<FRenderComponent, T>,
	              "Object Can Only Get Component By This Function");
	PObject->AddComponent(new T);
}



template <typename T>
Ptr<T> Engine::IO::LoadAsset(const wstring& PRelativeFilePath)
{
	static_assert(std::is_base_of_v<FAsset, T>, "Only Asset Can Loaded By This Function");
	return FAssetManager::GetInst()->Load<T>(PRelativeFilePath);
}
