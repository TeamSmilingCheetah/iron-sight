#include "pch.h"
#include "Engine/API/Public/EngineFacade.h"

#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/System/Public/Manager/CollisionManager.h"

#include "Engine/Runtime/Public/Component/Base/components.h"

#include "Engine/System/Public/Manager/CStateMgr.h"

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

FComponent* Engine::Common::GetComponent(const CGameObject* PObject, COMPONENT_TYPE PComponentType)
{
	return PObject->GetComponent(PComponentType);
}

void Engine::Common::AddChild(CGameObject* PObject, CGameObject* PChild)
{
	PObject->AddChild(PChild);
}


CState* Engine::Common::LoadState(const wstring& PStateName)
{
	return CStateMgr::GetInst()->GetState(PStateName);
}

void Engine::Common::AddScriptToObject(CGameObject* PObject, SCRIPT_TYPE PScriptType)
{
	CScript* Script = CScriptMgr::GetInst()->GetScript(PScriptType);

	PObject->AddComponent(Script);
}


void Engine::Layer::SetLayerName(CLevel* PLevel, int PLayerNumber, const ::wstring& PLayerName)
{
	PLevel->GetLayer(PLayerNumber)->SetName(PLayerName);
}

void Engine::Layer::SetLayerCollision(int PLayer, int POtherLayer)
{
	FCollisionManager::GetInst()->ActiveLayerCollision(PLayer, POtherLayer);
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

void Engine::Collider::SetColliderProperties(CGameObject* PObject, Vec3 PScale, Vec3 POffset,
                                          bool PIsIndependentScale, bool PIsTrigger)
{
	PObject->BoxCollider()->SetScale(PScale);
	PObject->BoxCollider()->SetOffset(POffset);
	PObject->BoxCollider()->SetIndependentScale(PIsIndependentScale);
	PObject->BoxCollider()->SetTrigger(PIsTrigger);
}

void Engine::Collider::SetRayColliderProperties(CGameObject* InObject,
                                             Vec3 InDirection, float InLength, bool InIsIndependant)
{
	InObject->RayCollider()->SetDirection(InDirection);
	InObject->RayCollider()->SetLength(InLength);
	InObject->RayCollider()->SetIndependentDir(InIsIndependant);
}

void Engine::Collider::SetRayColliderProperties(CGameObject* InObject,
                                             Vec3 InDirection, Vec3 InOffset, float InLength, bool InIsTrigger)
{
	InObject->RayCollider()->SetDirection(InDirection);
	InObject->RayCollider()->SetOffset(InOffset);
	InObject->RayCollider()->SetLength(InLength);
	InObject->RayCollider()->SetTriggerTarget(InIsTrigger);
}

void Engine::Collider::SetColliderDynamic(const CGameObject* InObject, EColliderType InType)
{
	for (auto ChildObject : InObject->GetChild())
	{
		SetColliderDynamic(ChildObject, InType);
	}

	for (auto Variant : InObject->GetColliders())
	{
		IColliderBase* Collider = GetBaseFromVariant(Variant);
		if (Collider->GetColliderType() == InType)
		{
			Collider->SetDynamic();
		}
	}
}

void Engine::Transform::SetPosition(CGameObject* PObject, Vec3 PPosition)
{
	PObject->Transform()->SetRelativePos(PPosition);
}

void Engine::Transform::SetPositionAndRotation(CGameObject* PObject, Vec3 PPosition, Vec3 PRotation)
{
	PObject->Transform()->SetRelativePos(PPosition);
	PObject->Transform()->SetRelativeRotation(PRotation);
}

void Engine::Transform::SetScale(CGameObject* PObject, Vec3 PScale)
{
	PObject->Transform()->SetRelativeScale(PScale);
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

void Engine::Light::Set3DLightProperties(CGameObject* PLightObject, LIGHT_TYPE PLightType, Vec3 PLightColor,
                                         Vec3 PAmbient,
                                         float PSpecularCoefficient, float PRadius)
{
	PLightObject->Light3D()->SetLightType(PLightType);
	PLightObject->Light3D()->SetLightColor(PLightColor);
	PLightObject->Light3D()->SetAmbient(PAmbient);
	PLightObject->Light3D()->SetSpecularCoefficient(PSpecularCoefficient);
	PLightObject->Light3D()->SetRadius(PRadius);
}

void Engine::Landscape::SetLandscapeProperties(CGameObject* PLandscape, UnsignedIntegerSquare PFaceSize,
                                               UnsignedIntegerSquare PHeightMapSize, Ptr<CTexture> PColorTexture,
                                               Ptr<CTexture> PNormalTexture)
{
	PLandscape->Landscape()->SetFace(PFaceSize.X, PFaceSize.Y);
	PLandscape->Landscape()->CreateHeightMap(PHeightMapSize.X, PHeightMapSize.Y);
	// PLandscape->LandScape()->SetColorTexture(PColorTexture);
	// PLandscape->LandScape()->SetNormalTexture(PNormalTexture);
}

Ptr<CMeshData> Engine::IO::LoadFBX(const wstring& PRelativeFilePath)
{
	return FAssetManager::GetInst()->LoadFBX(PRelativeFilePath.c_str());
}

void Engine::Animation::SetAnimationClips(CGameObject* PObject, Ptr<CMeshData> PAnimationSet)
{
	PObject->Animator3D()->SetAnimClips(PAnimationSet->GetAnimations());
}

void Engine::Animation::AddAnimationClips(CGameObject* PObject, Ptr<CMeshData> PAnimationSet)
{
	PObject->Animator3D()->AddAnimClips(PAnimationSet->GetAnimations());
}

void Engine::Animation::AddAnimationClip(CGameObject* PObject, Ptr<CAnimation> PAnimation)
{
	PObject->Animator3D()->AddAnimClip(PAnimation);
}

void Engine::StateMachine::AddState(CGameObject* PObject, CState* PState)
{
	PObject->StateMachine()->AddState(PState);
}

void Engine::StateMachine::AddTransition(CGameObject* PObject, const wstring& PFrom, const wstring& PTo)
{
	PObject->StateMachine()->AddTransition(PFrom, PTo);
}

void Engine::StateMachine::AddAnyTransition(CGameObject* PObject, const wstring& PTo)
{
	PObject->StateMachine()->AddAnyTransition(PTo);
}

void Engine::StateMachine::SetDefaultState(CGameObject* PObject, const wstring& PStateName)
{
	PObject->StateMachine()->SetDefaultState(PStateName);
}
