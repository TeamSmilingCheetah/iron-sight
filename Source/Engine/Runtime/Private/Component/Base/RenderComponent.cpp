#include "pch.h"
#include "Engine/Runtime/Public/Component/Base/RenderComponent.h"

#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"

FRenderComponent::FRenderComponent(COMPONENT_TYPE InType)
    : FComponent(InType)
{
}

FRenderComponent::FRenderComponent(const FRenderComponent& InOrigin)
    : FComponent(InOrigin)
      , Mesh(InOrigin.Mesh)
{
    MaterialSetVector.resize(InOrigin.MaterialSetVector.size());

    for (size_t i = 0; i < InOrigin.MaterialSetVector.size(); ++i)
    {
        MaterialSetVector[i].pCurMtrl = InOrigin.MaterialSetVector[i].pCurMtrl;
        MaterialSetVector[i].pSharedMtrl = InOrigin.MaterialSetVector[i].pSharedMtrl;

        // 원본 오브젝트가 공유재질을 참조하고 있고, 현재 사용재질은 공유재질이 아닌경우
        if (InOrigin.MaterialSetVector[i].pSharedMtrl != InOrigin.MaterialSetVector[i].pCurMtrl)
        {
            assert(InOrigin.MaterialSetVector[i].pDynamicMtrl.Get());

            // 복사 렌더 컴포넌트도 별도의 동적재질을 생성한다.
            GetDynamicMaterial(static_cast<UINT>(i));

            // 원본 렌더컴포넌트의 동적재질 값을 현재 생성한 동적재질로 복사한다.
            *MaterialSetVector[i].pDynamicMtrl.Get() = *InOrigin.MaterialSetVector[i].pDynamicMtrl.Get();
        }
        else
        {
            MaterialSetVector[i].pCurMtrl = MaterialSetVector[i].pSharedMtrl;
        }
    }
}

FRenderComponent::~FRenderComponent() = default;

void FRenderComponent::Render(UINT InSubset)
{
	Render();
}

ULONG64 FRenderComponent::GetInstID(UINT InMaterialIdx) const
{
	if (Mesh == nullptr || MaterialSetVector[InMaterialIdx].pCurMtrl == nullptr)
		return 0;

	uInstID id{ (UINT)Mesh->GetID(), (WORD)MaterialSetVector[InMaterialIdx].pCurMtrl->GetID(), (WORD)InMaterialIdx };
	return id.llID;
}

void FRenderComponent::SetMesh(Ptr<CMesh> InMesh)
{
    Mesh = InMesh;

    if (!MaterialSetVector.empty())
    {
        MaterialSetVector.clear();
        vector<tMtrlSet> vecMtrls;
        MaterialSetVector.swap(vecMtrls);
    }

    if (nullptr != Mesh)
        MaterialSetVector.resize(Mesh->GetSubsetCount());
}

void FRenderComponent::SetMaterial(Ptr<CMaterial> InMaterial, UINT InIdx)
{
    // 재질이 변경되면 기존에 복사본 받아둔 DynamicMaterial 을 삭제한다.
    MaterialSetVector[InIdx].pSharedMtrl = InMaterial;
    MaterialSetVector[InIdx].pCurMtrl = InMaterial;
    MaterialSetVector[InIdx].pDynamicMtrl = nullptr;
}

Ptr<CMaterial> FRenderComponent::GetMaterial(UINT InIdx)
{
	// 가져오는 조건추가, 가진 재질수가0이거나 이상한 번호를 가져오면 nullptr반환
	if (MaterialSetVector.size() == 0 || MaterialSetVector.size() < InIdx)
		return nullptr;

    return MaterialSetVector[InIdx].pCurMtrl;
}

Ptr<CMaterial> FRenderComponent::GetSharedMaterial(UINT InIdx)
{
    // 공유재질을 가져오는것으로 현재 사용재질을 동적재질에서 회복하도록 한다
    MaterialSetVector[InIdx].pCurMtrl = MaterialSetVector[InIdx].pSharedMtrl;

    if (MaterialSetVector[InIdx].pDynamicMtrl.Get())
    {
        MaterialSetVector[InIdx].pDynamicMtrl = nullptr;
    }

    return MaterialSetVector[InIdx].pSharedMtrl;
}

Ptr<CMaterial> FRenderComponent::GetDynamicMaterial(UINT InIdx)
{
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
    if (pCurLevel->GetState() != LEVEL_STATE::PLAY)
        return nullptr;

    // 원본 재질이 없다 -> Nullptr 반환
    if (nullptr == MaterialSetVector[InIdx].pSharedMtrl)
    {
        MaterialSetVector[InIdx].pCurMtrl = nullptr;
        MaterialSetVector[InIdx].pDynamicMtrl = nullptr;
        return MaterialSetVector[InIdx].pCurMtrl;
    }

    if (nullptr == MaterialSetVector[InIdx].pDynamicMtrl)
    {
        MaterialSetVector[InIdx].pDynamicMtrl = MaterialSetVector[InIdx].pSharedMtrl->Clone();
        MaterialSetVector[InIdx].pDynamicMtrl->SetName(MaterialSetVector[InIdx].pSharedMtrl->GetName() + L"_Clone");
        MaterialSetVector[InIdx].pCurMtrl = MaterialSetVector[InIdx].pDynamicMtrl;
    }

    return MaterialSetVector[InIdx].pCurMtrl;
}

void FRenderComponent::SaveComponent(FILE* InFile)
{
    // 메쉬 참조정보 저장
    SaveAssetRef(Mesh, InFile);

    // 재질 참조정보 저장
    UINT MaterialCount = GetMaterialCount();
    (void)fwrite(&MaterialCount, sizeof(UINT), 1, InFile);

    for (UINT i = 0; i < MaterialCount; ++i)
    {
        SaveAssetRef(MaterialSetVector[i].pSharedMtrl, InFile);
    }
}

void FRenderComponent::LoadComponent(FILE* InFile)
{
    // 메쉬 참조정보 불러오기
    LoadAssetRef(Mesh, InFile);

    // 재질 참조정보 불러오기
    UINT MaterialCount = GetMaterialCount();
    (void)fread(&MaterialCount, sizeof(UINT), 1, InFile);

	MaterialSetVector.resize(MaterialCount);

    for (UINT i = 0; i < MaterialCount; ++i)
    {
        LoadAssetRef(MaterialSetVector[i].pSharedMtrl, InFile);
		SetMaterial(MaterialSetVector[i].pSharedMtrl, i);
    }
}
