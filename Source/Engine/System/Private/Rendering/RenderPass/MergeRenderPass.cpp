#include "pch.h"
#include "Engine/System/Public/Rendering/RenderPass/MergeRenderPass.h"

#include "System/Public/Manager/RenderManager.h"

void FMergeRenderPass::Execute(FRenderPassParameters& InParams)
{
	Ptr<CMaterial> MergeMaterial = GetMaterial(0);
	Ptr<CMesh> RectMesh = GetMesh(0);

	if (!MergeMaterial.Get() || !RectMesh.Get())
	{
		assert(!"Can't Execute Merge Render Pass");
		return;
	}

	FRenderManager* RenderManager = FRenderManager::GetInst();

	// Geometry Pass
	FMultiRenderTarget* DeferredMRT = RenderManager->GetRenderTarget(MRT_TYPE::DEFERRED);
	Ptr<CTexture> ColorTargetTexture = DeferredMRT->GetTexture(0);
	Ptr<CTexture> WorldPositionTargetTexture = DeferredMRT->GetTexture(2);
	Ptr<CTexture> EmissiveTargetTexture = DeferredMRT->GetTexture(3);

	// Light Pass
	FMultiRenderTarget* LightMRT = RenderManager->GetRenderTarget(MRT_TYPE::LIGHT);
	Ptr<CTexture> DiffuseTargetTexture = LightMRT->GetTexture(0);
	Ptr<CTexture> SpecularTargetTexture = LightMRT->GetTexture(1);

	// Set Texture As Param
	MergeMaterial->SetTexParam(TEX_0, ColorTargetTexture);
	MergeMaterial->SetTexParam(TEX_1, WorldPositionTargetTexture);
	MergeMaterial->SetTexParam(TEX_2, DiffuseTargetTexture);
	MergeMaterial->SetTexParam(TEX_3, SpecularTargetTexture);
	MergeMaterial->SetTexParam(TEX_4, EmissiveTargetTexture);

	// Set Merge Flag
	MergeMaterial->SetScalarParam(INT_0, 0);

	// Material Binding & Rendering
	MergeMaterial->Binding();
	RectMesh->Render(0);

	// Clear Texture Slot
	CTexture::Clear(0, 8);
}

void FMergeRenderPass::ExecuteMinimap() const
{
	Ptr<CMaterial> MergeMaterial = GetMaterial(0);
	Ptr<CMesh> RectMesh = GetMesh(0);

	if (!MergeMaterial.Get())
	{
		return;
	}

	FMultiRenderTarget* DeferredMRT = FRenderManager::GetInst()->GetRenderTarget(MRT_TYPE::DEFERRED);
	Ptr<CTexture> ColorTargetTexture = DeferredMRT->GetTexture(0);

	MergeMaterial->SetTexParam(TEX_0, ColorTargetTexture);
	MergeMaterial->Binding();

	RectMesh->Render(0);
	CTexture::Clear(0);
}
