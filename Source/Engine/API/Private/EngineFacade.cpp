#include "pch.h"
#include "Engine/API/Public/EngineFacade.h"

#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/System/Public/Manager/CCollisionMgr.h"
#include "Runtime/Public/Component/Camera/CCamera.h"
#include "Runtime/Public/Component/Physics/CColliderRay.h"

std::unique_ptr<CGameObject> Engine::Common::CreateNewObject()
{
	return std::make_unique<CGameObject>();
}

void Engine::Common::SetObjectName(CGameObject* PObject, const wstring& PName)
{
	PObject->SetName(PName);
}

CComponent* Engine::Common::GetComponent(const CGameObject* PObject, COMPONENT_TYPE PComponentType)
{
	return PObject->GetComponent(PComponentType);
}

void Engine::Layer::SetLayerName(CLevel* PLevel, int PLayerNumber, const ::wstring& PLayerName)
{
	PLevel->GetLayer(PLayerNumber)->SetName(PLayerName);
}

void Engine::Layer::SetLayerCollision(int PLayer, int POtherLayer)
{
	CCollisionMgr::GetInst()->ToggleLayerCollision(PLayer, POtherLayer);
}

std::unique_ptr<CLevel> Engine::Level::CreateNewLevel()
{
	return std::make_unique<CLevel>();
}

void Engine::Level::AddObjectToLayer(CLevel* PLevel, CGameObject* PObject, int PLayerIdx, bool PMoveWithChild)
{
	PLevel->AddObject(PLayerIdx, PObject, PMoveWithChild);
}

void Engine::Camera::SetCameraOptions(CGameObject* CameraObject, PROJ_TYPE PProjectionType, int PPriority)
{
	CameraObject->Camera()->SetProjType(PProjectionType);
	CameraObject->Camera()->SetPriority(PPriority);

	// TPS Layer 제외하고 Init
	CameraObject->Camera()->LayerCheckAll();
	CameraObject->Camera()->LayerOff(4);
}


void Engine::Collider::SetColliderRayOptions(CGameObject* ColliderObject,
                                             Vec3 PDirection, float PLength, bool PIsIndependant)
{
	ColliderObject->ColliderRay()->SetRayDir(PDirection);
	ColliderObject->ColliderRay()->SetRayLength(PLength);
	ColliderObject->ColliderRay()->SetIndependentDir(PIsIndependant);
}
