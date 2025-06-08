#include "pch.h"
#include "Client/UI/Public/Asset/AssetUI.h"

#include "Engine/System/Public/Asset/Base/assets.h"

AssetUI::AssetUI(const string& _ID, ASSET_TYPE _Type)
	: EditorUI(_ID)
	  , m_Type(_Type)
{
	SetActive(false);
}

AssetUI::~AssetUI()
{
}

void AssetUI::AssetTitle()
{
	ImGui::PushID(0);

	ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.2f, 0.6f, 0.6f)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
	                      static_cast<ImVec4>(ImColor::HSV(0.2f, 0.6f, 0.6f)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,
	                      static_cast<ImVec4>(ImColor::HSV(0.2f, 0.6f, 0.6f)));

	ImGui::Button(ASSET_TYPE_STRING[static_cast<UINT>(m_Type)]);

	ImGui::PopStyleColor(3);
	ImGui::PopID();
}

void AssetUI::SaveButton()
{
	// Engine Resource는 저장할 수 없다.
	if (m_TargetAsset->IsEngineAsset())
		return;

	if (ImGui::Button("SAVE"))
	{
		wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + m_TargetAsset->GetKey();
		m_TargetAsset->Save(strFilePath);
	}
}

void AssetUI::Deactivate()
{
	m_TargetAsset = nullptr;
}
