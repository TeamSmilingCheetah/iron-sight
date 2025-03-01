#include "pch.h"
#include "Core/Public/CEntity.h"

UINT CEntity::g_NextID = 0;

CEntity::CEntity()
    : m_ID(g_NextID++)
{
}

CEntity::CEntity(const CEntity& _Origin)
    : m_Name(_Origin.m_Name)
      , m_ID(g_NextID++)
{
}

CEntity::~CEntity()
{
}

void CEntity::SaveToLevel(FILE* _File)
{
    SaveWString(m_Name, _File);
}

void CEntity::LoadFromLevel(FILE* _File)
{
    LoadWString(m_Name, _File);
}
