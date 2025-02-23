#pragma once
#include "CComponent.h"

class CLight2D :
    public CComponent
{
private:
    tLight2DInfo    m_LightInfo;

public:
    void SetLightColor(Vec3 _Color) { m_LightInfo.vColor = _Color; }  
    void SetRadius(float _Radius) { m_LightInfo.Radius = _Radius; }
    void SetAngle(float _Angle) { m_LightInfo.Angle = _Angle; }
    void SetLightType(LIGHT_TYPE _Type) { m_LightInfo.Type = (int)_Type; }

    const tLight2DInfo& GetLight2DInfo() { return m_LightInfo; }

public:
    virtual void FinalTick() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;
public:
    CLONE(CLight2D);
    CLight2D();
    ~CLight2D();
};

