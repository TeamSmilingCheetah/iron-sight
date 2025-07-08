#pragma once
#include "Common/global.h"
#include <type_traits>

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

/**
 * @brief Game Contents Develop에 필요한 Engine API를 간접호출할 수 있도록 만든 namespace
 */
namespace Engine
{
	namespace Common
	{
		unique_ptr<CGameObject> CreateNewObject();
		void SetObjectName(CGameObject* PObject, const wstring& PName);
		CComponent* GetComponent(const CGameObject* PObject, COMPONENT_TYPE PComponentType);

		template <class T>
		void AddComponentToObject(CGameObject* PObject);

		template <class T>
		void AddScriptToObject(CGameObject* PObject);
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
		void SetCameraOptions(CGameObject* CameraObject, PROJ_TYPE PProjectionType, int PPriority);
	}

	namespace Collider
	{
		void SetColliderRayOptions(CGameObject* ColliderObject,
		                           Vec3 PDirection, float PLength, bool PIsIndependant);
	}
};

template <class T>
void Engine::Common::AddComponentToObject(CGameObject* PObject)
{
	static_assert(std::is_base_of_v<CComponent, T>, "Object Can Only Get Component By This Function");
	PObject->AddComponent(new T);
}

template <class T>
void Engine::Common::AddScriptToObject(CGameObject* PObject)
{
	static_assert(std::is_base_of_v<CScript, T>, "Object Can Only Get Script By This Function");
	PObject->AddComponent(new T);
}
