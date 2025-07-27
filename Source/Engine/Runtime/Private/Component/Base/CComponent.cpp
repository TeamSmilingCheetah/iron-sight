#include "pch.h"
#include "Runtime/Public/Component/Base/CComponent.h"

#include "Runtime/Public/Actor/CGameObject.h"

CComponent::CComponent(COMPONENT_TYPE _TYPE)
    : m_Owner(nullptr)
    , m_Type(_TYPE)
{
}

CComponent::~CComponent()
{
}

void CComponent::SaveToLevel(FILE* _File)
{
    CEntity::SaveToLevel(_File);

    SaveComponent(_File);
}

void CComponent::LoadFromLevel(FILE* _File)
{
    CEntity::LoadFromLevel(_File);

    LoadComponent(_File);
}

// Getter Function
CTransform* CComponent::Transform() const { return m_Owner ? m_Owner->Transform() : nullptr; }
CMeshRender* CComponent::MeshRender() const { return m_Owner ? m_Owner->MeshRender() : nullptr; }
CCamera* CComponent::Camera() const { return m_Owner ? m_Owner->Camera() : nullptr; }
FCollider2D* CComponent::Collider2D() const { return m_Owner ? m_Owner->Collider2D() : nullptr; }
FCollider3D* CComponent::Collider3D() const { return m_Owner ? m_Owner->Collider3D() : nullptr; }
CColliderRay* CComponent::ColliderRay() const { return m_Owner ? m_Owner->ColliderRay() : nullptr; }
CFlipbookPlayer* CComponent::FlipbookPlayer() const { return m_Owner ? m_Owner->FlipbookPlayer() : nullptr; }
CTileMap* CComponent::TileMap() const { return m_Owner ? m_Owner->TileMap() : nullptr; }
CLight2D* CComponent::Light2D() const { return m_Owner ? m_Owner->Light2D() : nullptr; }
CLight3D* CComponent::Light3D() const { return m_Owner ? m_Owner->Light3D() : nullptr; }
CAnimator3D* CComponent::Animator3D() const { return m_Owner ? m_Owner->Animator3D() : nullptr; }
CParticleSystem* CComponent::ParticleSystem() const { return m_Owner ? m_Owner->ParticleSystem() : nullptr; }
CSkyBox* CComponent::SkyBox() const { return m_Owner ? m_Owner->SkyBox() : nullptr; }
CDecal* CComponent::Decal() const { return m_Owner ? m_Owner->Decal() : nullptr; }
CLandScape* CComponent::LandScape() const { return m_Owner ? m_Owner->LandScape() : nullptr; }
CUI* CComponent::UI() const { return m_Owner ? m_Owner->UI() : nullptr; }
CUIRender* CComponent::UIRender() const { return m_Owner ? m_Owner->UIRender() : nullptr; }
FMeshCollider* CComponent::MeshCollider() const { return m_Owner ? m_Owner->MeshCollider() : nullptr; }
