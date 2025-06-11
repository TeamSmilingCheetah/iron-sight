#include "pch.h"
#include "Client/UI/Public/Asset/AssetUI.h"

#include "Engine/System/Public/Asset/Base/assets.h"

AssetUI::AssetUI(const string& _ID, ASSET_TYPE _Type)
	: EditorUI(_ID)
	, m_Type(_Type)
	, m_DirtyFlag(false)
{
	SetActive(false);
}

AssetUI::~AssetUI()
{
}

void AssetUI::SetAsset(Ptr<CAsset> _Asset)
{
	if (m_TargetAsset == _Asset)
		return;

	m_TargetAsset = _Asset;
	m_DirtyFlag = true;
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
		// TEST : Engine Resource가 아닌 경우 key와 relative path가 동일하다고 가정
		assert(m_TargetAsset->GetKey() == m_TargetAsset->GetRelativePath());
		m_TargetAsset->Save(m_TargetAsset->GetKey());
	}
}

void AssetUI::Deactivate()
{
	m_TargetAsset = nullptr;
}

void AssetUI::Tick()
{
	if (!IsActive())
		return;

	// TEST : DirtyFlag가 Render단계에서 설정되므로 다음 프레임 Tick에서 해제해줌
	m_DirtyFlag = false;
}
