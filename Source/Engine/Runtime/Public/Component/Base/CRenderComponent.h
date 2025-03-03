#pragma once
#include "CComponent.h"
#include "Engine/System/Public/Asset/Mesh/CMesh.h"
#include "Engine/System/Public/Rendering/Material/CMaterial.h"

struct tMtrlSet
{
    Ptr<CMaterial> pSharedMtrl; // 공유 메테리얼
    Ptr<CMaterial> pDynamicMtrl; // 공유 메테리얼의 복사본
    Ptr<CMaterial> pCurMtrl; // 현재 사용 할 메테리얼
};

class CRenderComponent :
    public CComponent
{
    Ptr<CMesh> m_Mesh;
    vector<tMtrlSet> m_vecMtrls; // 재질

public:
    void SetMesh(Ptr<CMesh> _Mesh);
    Ptr<CMesh> GetMesh() { return m_Mesh; }

    void SetMaterial(Ptr<CMaterial> _Mtrl, UINT _idx);

    Ptr<CMaterial> GetMaterial(UINT _idx);
    Ptr<CMaterial> GetSharedMaterial(UINT _idx);
    Ptr<CMaterial> GetDynamicMaterial(UINT _idx);

    UINT GetMaterialCount() { return static_cast<UINT>(m_vecMtrls.size()); }

	ULONG64 GetInstID(UINT _iMtrlIdx);

    virtual void Render() = 0;
	virtual void Render(UINT _Subset);
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _FILE) override;

    CRenderComponent* Clone() override = 0;

    CRenderComponent(COMPONENT_TYPE _Type);
    CRenderComponent(const CRenderComponent& _Origin);
    ~CRenderComponent() override;
};
