#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"
#include "Engine/System/Public/Asset/Mesh/CMesh.h"
#include "Engine/System/Public/Rendering/Meterial/CMaterial.h"

class CLight3D :
    public CComponent
{
    tLight3DInfo m_LightInfo;
    int m_LightIdx;

    Ptr<CMesh> m_VolumeMesh;
    Ptr<CMaterial> m_LightMtrl;

public:
    void SetLightType(LIGHT_TYPE _Type);

    void SetLightColor(Vec3 _Color) { m_LightInfo.info.vColor = _Color; }
    void SetAmbient(Vec3 _Amb) { m_LightInfo.info.vAmbient = _Amb; }
    void SetSpecularCoefficient(float _Spec) { m_LightInfo.info.SpecCoeff = _Spec; }

    void SetRadius(float _Radius);
    void SetAngle(float _Angle) { m_LightInfo.Angle = _Angle; }

    const tLight3DInfo& GetLight3DInfo() { return m_LightInfo; }

    void FinalTick() override;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _File) override;

    void Render();


    CLONE(CLight3D);
    CLight3D();
    ~CLight3D() override;
};
