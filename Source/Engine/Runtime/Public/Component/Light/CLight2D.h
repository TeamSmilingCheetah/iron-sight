#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

class CLight2D :
    public CComponent
{
    tLight2DInfo m_LightInfo;

public:
    void SetLightColor(Vec3 _Color) { m_LightInfo.vColor = _Color; }
    void SetRadius(float _Radius) { m_LightInfo.Radius = _Radius; }
    void SetAngle(float _Angle) { m_LightInfo.Angle = _Angle; }
    void SetLightType(LIGHT_TYPE _Type) { m_LightInfo.Type = static_cast<int>(_Type); }

    const tLight2DInfo& GetLight2DInfo() { return m_LightInfo; }

    void FinalTick() override;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _File) override;
    CLONE(CLight2D);
    CLight2D();
    ~CLight2D() override;
};
