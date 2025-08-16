#pragma once
#include "Component.h"
#include "Engine/System/Public/Asset/Mesh/CMesh.h"
#include "Engine/System/Public/Rendering/Material/CMaterial.h"

struct tMtrlSet
{
    Ptr<CMaterial> pSharedMtrl; // 공유 메테리얼
    Ptr<CMaterial> pDynamicMtrl; // 공유 메테리얼의 복사본
    Ptr<CMaterial> pCurMtrl; // 현재 사용 할 메테리얼
};

class FRenderComponent :
    public FComponent
{
    Ptr<CMesh> Mesh;
    vector<tMtrlSet> MaterialSetVector; // 재질

public:
	virtual void Render() = 0;
	virtual void Render(UINT InSubset);

	// Getter & Setter
	Ptr<CMesh> GetMesh() { return Mesh; }
	Ptr<CMaterial> GetMaterial(UINT InIdx);
	Ptr<CMaterial> GetSharedMaterial(UINT InIdx);
	Ptr<CMaterial> GetDynamicMaterial(UINT InIdx);
	ULONG64 GetInstID(UINT InMaterialIdx) const;
	UINT GetMaterialCount() const { return static_cast<UINT>(MaterialSetVector.size()); }

    void SetMesh(Ptr<CMesh> InMesh);
    void SetMaterial(Ptr<CMaterial> InMaterial, UINT InIdx);
	void SetMaterialSize(UINT InSize) { MaterialSetVector.resize(InSize); }

	// Save & Load
    void SaveComponent(FILE* InFile) override;
    void LoadComponent(FILE* InFile) override;

	// Special Member Function
    FRenderComponent(COMPONENT_TYPE InType);
    FRenderComponent(const FRenderComponent& InOrigin);
    ~FRenderComponent() override;
	FRenderComponent* Clone() override = 0;
};
