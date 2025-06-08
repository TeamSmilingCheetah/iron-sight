#pragma once
#include "Engine/Core/Public/CEntity.h"

class CCamera;

class CFrustum :
    public CEntity
{
    CCamera* m_Owner;

    Vec3 m_ProjPos[8];
    Vec4 m_Face[static_cast<int>(FACE_TYPE::END)];

    void SetOwner(CCamera* _Owner) { m_Owner = _Owner; }

public:
    bool IsInFrustum(const Vec3& _WorldPos);

    void FinalTick();

    CLONE(CFrustum);
    CFrustum();
    ~CFrustum() override;

    friend class CCamera;
};
