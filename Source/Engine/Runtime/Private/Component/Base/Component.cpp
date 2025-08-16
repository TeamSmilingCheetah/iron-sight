#include "pch.h"
#include "Runtime/Public/Component/Base/Component.h"

#include "Runtime/Public/Actor/CGameObject.h"

FComponent::FComponent(COMPONENT_TYPE InType)
    : Owner(nullptr)
    , Type(InType)
{
}

FComponent::~FComponent()
{
}

void FComponent::SaveToLevel(FILE* InFile)
{
    CEntity::SaveToLevel(InFile);

    SaveComponent(InFile);
}

void FComponent::LoadFromLevel(FILE* InFile)
{
    CEntity::LoadFromLevel(InFile);

    LoadComponent(InFile);
}

// Getter Function
CTransform* FComponent::Transform() const { return Owner ? Owner->Transform() : nullptr; }
CMeshRender* FComponent::MeshRender() const { return Owner ? Owner->MeshRender() : nullptr; }
CCamera* FComponent::Camera() const { return Owner ? Owner->Camera() : nullptr; }
FPlaneCollider* FComponent::PlaneCollider() const { return Owner ? Owner->PlaneCollider() : nullptr; }
FBoxCollider* FComponent::BoxCollider() const { return Owner ? Owner->BoxCollider() : nullptr; }
FRayCollider* FComponent::RayCollider() const { return Owner ? Owner->RayCollider() : nullptr; }
CFlipbookPlayer* FComponent::FlipbookPlayer() const { return Owner ? Owner->FlipbookPlayer() : nullptr; }
CTileMap* FComponent::TileMap() const { return Owner ? Owner->TileMap() : nullptr; }
CLight2D* FComponent::Light2D() const { return Owner ? Owner->Light2D() : nullptr; }
CLight3D* FComponent::Light3D() const { return Owner ? Owner->Light3D() : nullptr; }
CAnimator3D* FComponent::Animator3D() const { return Owner ? Owner->Animator3D() : nullptr; }
CParticleSystem* FComponent::ParticleSystem() const { return Owner ? Owner->ParticleSystem() : nullptr; }
CSkyBox* FComponent::SkyBox() const { return Owner ? Owner->SkyBox() : nullptr; }
CDecal* FComponent::Decal() const { return Owner ? Owner->Decal() : nullptr; }
FLandscape* FComponent::LandScape() const { return Owner ? Owner->LandScape() : nullptr; }
CUI* FComponent::UI() const { return Owner ? Owner->UI() : nullptr; }
CUIRender* FComponent::UIRender() const { return Owner ? Owner->UIRender() : nullptr; }
FMeshCollider* FComponent::MeshCollider() const { return Owner ? Owner->MeshCollider() : nullptr; }
CStateMachine* FComponent::StateMachine() const { return Owner ? Owner->StateMachine() : nullptr; }
