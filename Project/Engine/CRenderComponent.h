#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

struct tMtrlSet
{
    Ptr<CMaterial> pSharedMtrl; // ���� ���׸���
    Ptr<CMaterial> pDynamicMtrl; // ���� ���׸����� ���纻    
    Ptr<CMaterial> pCurMtrl; // ���� ��� �� ���׸���
};

class CRenderComponent :
    public CComponent
{
    Ptr<CMesh> m_Mesh;
    vector<tMtrlSet> m_vecMtrls; // ���� 


public:
    void SetMesh(Ptr<CMesh> _Mesh);
    Ptr<CMesh> GetMesh() { return m_Mesh; }

    void SetMaterial(Ptr<CMaterial> _Mtrl, UINT _idx);

    Ptr<CMaterial> GetMaterial(UINT _idx);
    Ptr<CMaterial> GetSharedMaterial(UINT _idx);
    Ptr<CMaterial> GetDynamicMaterial(UINT _idx);

    UINT GetMaterialCount() { return static_cast<UINT>(m_vecMtrls.size()); }


    virtual void Render() = 0;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _FILE) override;


    CRenderComponent* Clone() override = 0;

    CRenderComponent(COMPONENT_TYPE _Type);
    CRenderComponent(const CRenderComponent& _Origin);
    ~CRenderComponent() override;
};
