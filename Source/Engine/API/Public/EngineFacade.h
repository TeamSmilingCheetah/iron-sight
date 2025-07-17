#pragma once
#include "Common/global.h"
#include <type_traits>

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Base/CComponent.h"
#include "Engine/Runtime/Public/Component/Rendering/CSkyBox.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandscape.h"

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
		CComponent* GetComponent(const CGameObject* PObject, COMPONENT_TYPE PComponentType);
		void AddChild(CGameObject* PObject, CGameObject* PChild);

		template <class T>
		void AddComponentToObject(CGameObject* PObject);

		template <class T>
		void AddScriptToObject(CGameObject* PObject);
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
		void SetColliderRayProperties(CGameObject* PObject, Vec3 PDirection, Vec3 POffset,
		                           float PLength, bool PIsTriggerTarget);
		void SetColliderRayProperties(CGameObject* PObject, Vec3 PDirection,
		                           float PLength, bool PIsIndependant);
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
	}
}

template <class T>
void Engine::Common::AddComponentToObject(CGameObject* PObject)
{
	static_assert(std::is_base_of_v<CComponent, T> || std::is_base_of_v<CRenderComponent, T>,
	              "Object Can Only Get Component By This Function");
	PObject->AddComponent(new T);
}

template <class T>
void Engine::Common::AddScriptToObject(CGameObject* PObject)
{
	static_assert(std::is_base_of_v<CScript, T>, "Object Can Only Get Script By This Function");
	PObject->AddComponent(new T);
}

template <typename T>
Ptr<T> Engine::IO::LoadAsset(const wstring& PRelativeFilePath)
{
	static_assert(std::is_base_of_v<CAsset, T>, "Only Asset Can Loaded By This Function");
	return CAssetMgr::GetInst()->Load<T>(PRelativeFilePath);
}
