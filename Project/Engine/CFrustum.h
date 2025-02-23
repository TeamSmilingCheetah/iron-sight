#pragma once
#include "CEntity.h"


enum FACE_TYPE
{
    FT_NEAR,
    FT_FAR,
    FT_LEFT,
    FT_RIGHT,
    FT_TOP,
    FT_BOT,
    FT_END,
};


class CCamera;

class CFrustum :
    public CEntity
{
    CCamera* m_Owner;

    Vec3 m_ProjPos[8];
    Vec4 m_Face[FT_END];

    void SetOwner(CCamera* _Owner) { m_Owner = _Owner; }

public:
    bool FrustumCheck(Vec3 _WorldPos);
    bool FrustumCheckSphere(Vec3 _WorldCenter, float _Radius);


    void FinalTick();

    CLONE(CFrustum);
    CFrustum();
    ~CFrustum() override;

    friend class CCamera;
};
