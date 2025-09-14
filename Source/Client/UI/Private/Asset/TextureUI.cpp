#include "pch.h"
#include "Client/UI/Public/Asset/TextureUI.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"

class CTexture;

TextureUI::TextureUI()
	: AssetUI("Texture", TEXTURE)
{
}

TextureUI::~TextureUI()
{
}

void TextureUI::Render_Update()
{
	AssetTitle();

	Ptr<CTexture> pAsset = dynamic_cast<CTexture*>(GetAsset().Get());
	assert(pAsset.Get());

	ImGui::Text("Name");
	ImGui::SameLine(100);

	auto strKey = WStringToString(pAsset->GetKey());

	ImGui::InputText("##TexName", (char*)strKey.c_str(), strKey.length(),
	                 ImGuiInputTextFlags_ReadOnly);

	int width = pAsset->GetWidth();
	int height = pAsset->GetHeight();

	ImGui::Text("Width");
	ImGui::SameLine(100);
	ImGui::InputInt("##Width", &width, 0, 0, ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Height");
	ImGui::SameLine(100);
	ImGui::InputInt("##Height", &height, 0, 0, ImGuiInputTextFlags_ReadOnly);

	auto uv_min = ImVec2(0.0f, 0.0f);
	auto uv_max = ImVec2(1.0f, 1.0f);
	auto tint_col = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
	ImGui::Image(pAsset->GetSRV().Get(), ImVec2(150.f, 150.f), uv_min, uv_max, tint_col,
	             border_col);

	// MetaOpts
	ImGui::Text("sRGB");
	ImGui::SameLine(100);
	ImGui::Checkbox("##sRGB", &m_MetaOpts.sRGB);

	ImGui::Text("MipMap");
	ImGui::SameLine(100);
	ImGui::Checkbox("##mipGen", &m_MetaOpts.mipGen);

	// 옵션 저장 및 적용
	if (ImGui::Button("Save and Apply"))
	{
		pAsset->SetMetaOpts(m_MetaOpts);
	}

}

void TextureUI::OnSelected()
{
	Ptr<CTexture> pAsset = dynamic_cast<CTexture*>(GetAsset().Get());
	m_MetaOpts = pAsset->GetMetaOpts();	// 사본으로 받음
}
