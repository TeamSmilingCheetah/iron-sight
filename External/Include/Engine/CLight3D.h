#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

class CLight3D :
    public CComponent
{
private:
    tLight3DInfo    m_LightInfo;
    int             m_LightIdx;

    Ptr<CMesh>      m_VolumeMesh;
    Ptr<CMaterial>  m_LightMtrl;


public:
    void SetLightType(LIGHT_TYPE _Type);

    void SetLightColor(Vec3 _Color) { m_LightInfo.info.vColor = _Color; }
    void SetAmbient(Vec3 _Amb) { m_LightInfo.info.vAmbient = _Amb; }
    void SetSpecularCoefficient(float _Spec) { m_LightInfo.info.SpecCoeff = _Spec; }

    void SetRadius(float _Radius);
    void SetAngle(float _Angle) { m_LightInfo.Angle = _Angle; }

    const tLight3DInfo& GetLight3DInfo() { return m_LightInfo; }

public:
    virtual void FinalTick() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

    void Render();


public:
    CLONE(CLight3D);
    CLight3D();
    ~CLight3D();
};

