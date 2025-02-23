#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

struct tMtrlSet
{
    Ptr<CMaterial>  pSharedMtrl;    // 공유 메테리얼
    Ptr<CMaterial>  pDynamicMtrl;   // 공유 메테리얼의 복사본    
    Ptr<CMaterial>  pCurMtrl;       // 현재 사용 할 메테리얼
};

class CRenderComponent :
    public CComponent
{
private:
    Ptr<CMesh>          m_Mesh;
    vector<tMtrlSet>    m_vecMtrls; // 재질 


public:
    void SetMesh(Ptr<CMesh> _Mesh);
    Ptr<CMesh> GetMesh() { return m_Mesh; }

    void SetMaterial(Ptr<CMaterial> _Mtrl, UINT _idx);

    Ptr<CMaterial> GetMaterial(UINT _idx);
    Ptr<CMaterial> GetSharedMaterial(UINT _idx);
    Ptr<CMaterial> GetDynamicMaterial(UINT _idx);

    UINT GetMaterialCount() { return (UINT)m_vecMtrls.size(); }


public:
    virtual void Render() = 0;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _FILE) override;


public:
    virtual CRenderComponent* Clone() = 0;

    CRenderComponent(COMPONENT_TYPE _Type);
    CRenderComponent(const CRenderComponent& _Origin);
    ~CRenderComponent();
};