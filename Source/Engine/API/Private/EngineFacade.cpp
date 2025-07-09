#include "pch.h"
#include "Engine/API/Public/EngineFacade.h"

#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CCollisionMgr.h"
#include "Runtime/Public/Component/Light/CLight3D.h"
#include "Runtime/Public/Component/Rendering/CLandScape.h"
#include "Runtime/Public/Component/Rendering/CSkyBox.h"

unique_ptr<CGameObject> Engine::Common::CreateNewObject()
{
	return make_unique<CGameObject>();
}

CGameObject* Engine::Common::InstantiateNewObject(Ptr<CMeshData> PMeshData)
{
	return PMeshData->Instantiate();
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

unique_ptr<CLevel> Engine::Level::CreateNewLevel()
{
	return make_unique<CLevel>();
}

void Engine::Level::AddObjectToLayer(CLevel* PLevel, CGameObject* PObject, int PLayerIdx, bool PMoveWithChild)
{
	PLevel->AddObject(PLayerIdx, PObject, PMoveWithChild);
}

void Engine::Camera::SetCameraOptions(CGameObject* PCameraObject, PROJ_TYPE PProjectionType, int PPriority)
{
	PCameraObject->Camera()->SetProjType(PProjectionType);
	PCameraObject->Camera()->SetPriority(PPriority);

	// TPS Layer 제외하고 Init
	PCameraObject->Camera()->LayerCheckAll();
	PCameraObject->Camera()->LayerOff(4);
}

void Engine::Collider::SetColliderRayOptions(CGameObject* PColliderObject,
                                             Vec3 PDirection, float PLength, bool PIsIndependant)
{
	PColliderObject->ColliderRay()->SetRayDir(PDirection);
	PColliderObject->ColliderRay()->SetRayLength(PLength);
	PColliderObject->ColliderRay()->SetIndependentDir(PIsIndependant);
}

void Engine::Transform::SetPositionAndRotation(CGameObject* PObject, Vec3 PPosition, Vec3 PRotation)
{
	PObject->Transform()->SetRelativePos(PPosition);
	PObject->Transform()->SetRelativeRotation(PRotation);
}

void Engine::Transform::SetScale(CGameObject* PObject, Vec3 PScale)
{
	PObject->Transform()->SetRelativeRotation(PScale);
}

void Engine::Transform::SetFrustumCheck(CGameObject* PObject, bool PCheck)
{
	PObject->Transform()->SetFrustumCheck(PCheck);
}

void Engine::SkyBox::SetSkyBoxProperties(CGameObject* PSkyBox, SKYBOX_MODE PMode, Ptr<CTexture> PTexture)
{
	PSkyBox->SkyBox()->SetMode(PMode);
	PSkyBox->SkyBox()->SetSkyBoxTexture(PTexture);
}

void Engine::Light::Set3DLightProperties(CGameObject* PLightObject, LIGHT_TYPE PLightType, Vec3 PLightColor, Vec3 PAmbient,
                                      float PSpecularCoefficient, float PRadius)
{
	PLightObject->Light3D()->SetLightType(PLightType);
	PLightObject->Light3D()->SetLightColor(PLightColor);
	PLightObject->Light3D()->SetAmbient(PAmbient);
	PLightObject->Light3D()->SetSpecularCoefficient(PSpecularCoefficient);
	PLightObject->Light3D()->SetRadius(PRadius);
}

void Engine::Landscape::SetLandscapeProperties(CGameObject* PLandscape, UnsignedIntegerSquare PFaceSize,
	UnsignedIntegerSquare PHeightMapSize, Ptr<CTexture> PColorTexture, Ptr<CTexture> PNormalTexture)
{
	PLandscape->LandScape()->SetFace(PFaceSize.X, PFaceSize.Y);
	PLandscape->LandScape()->CreateHeightMap(PHeightMapSize.X, PHeightMapSize.Y);
	PLandscape->LandScape()->SetColorTexture(PColorTexture);
	PLandscape->LandScape()->SetNormalTexture(PNormalTexture);
}

Ptr<CMeshData> Engine::IO::LoadFBX(const wstring& PRelativeFilePath)
{
	return CAssetMgr::GetInst()->LoadFBX(PRelativeFilePath.c_str());
}
