#include "pch.h"
#include "Client/UI/Public/Component/DecalUI.h"

#include "Client/UI/Public/Editor/TreeUI.h"

#include "Engine/Runtime/Public/Component/Rendering/CDecal.h"
#include "Client/UI/Public/Editor/ContentUI.h"


DecalUI::DecalUI()
	: ComponentUI("DecalUI", COMPONENT_TYPE::DECAL)
{
}

DecalUI::~DecalUI()
{
}

void DecalUI::Render_Update()
{
	ComponentTitle("Decal");

	CDecal* pDecal = GetTargetObject()->Decal();

	Ptr<CTexture> pDecalTex = pDecal->GetDecalTex();
	float fGlobalAlpha = pDecal->GetGlobalAlpha();
	bool bAsLight = pDecal->IsAsLight();
	
	ImGui::Text("GlobalAlpha");
	ImGui::SameLine(100);
	ImGui::DragFloat("##GlobalAlpha", &fGlobalAlpha);
	pDecal->SetGloablAlpha(fGlobalAlpha);

	ImGui::Text("AsLight");
	ImGui::SameLine(100);
	ImGui::Checkbox("##AsLight", &bAsLight);
	pDecal->SetAsLight(bAsLight);

	auto uv_min = ImVec2(0.0f, 0.0f);
	auto uv_max = ImVec2(1.0f, 1.0f);
	auto tint_col = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);


	if (nullptr == pDecalTex)
		ImGui::Image(nullptr, ImVec2(100.f, 100.f), uv_min, uv_max, tint_col, border_col);
	else
		ImGui::Image(pDecalTex->GetSRV().Get(), ImVec2(100.f, 100.f), uv_min, uv_max, tint_col, border_col);

	if (ImGui::BeginDragDropTarget())
	{
		if (ImGui::AcceptDragDropPayload("ContentUI"))
		{
			const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
			TreeNode* pNode = *((TreeNode**)pPayload->Data);
			tFSNode* pFSNode = reinterpret_cast<tFSNode*>(pNode->GetData());
			Ptr<FAsset> pAsset = pFSNode->Asset;
			if (pAsset == nullptr)
				pAsset = ContentUI::LoadAsset(pFSNode);

			if (pAsset->GetAssetType() == ASSET_TYPE::TEXTURE)
			{
				pDecal->SetDecalTexture(((CTexture*)pAsset.Get()));
				ImGui::SetWindowFocus(nullptr);
			}
		}
		ImGui::EndDragDropTarget();
	}


	if (ImGui::Button("DELETE##Decal"))
	{
		DeleteComponent(COMPONENT_TYPE::DECAL);
	}
}
