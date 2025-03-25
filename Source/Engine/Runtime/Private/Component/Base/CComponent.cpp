#include "pch.h"
#include "Runtime/Public/Component/Base/CComponent.h"

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
