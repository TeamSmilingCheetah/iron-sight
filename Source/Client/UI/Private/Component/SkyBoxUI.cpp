#include "pch.h"
#include "Client/UI/Public/Component/SkyBoxUI.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Rendering/CSkyBox.h"

SkyBoxUI::SkyBoxUI()
	: ComponentUI("SkyBoxUI", COMPONENT_TYPE::SKYBOX)
{
}

SkyBoxUI::~SkyBoxUI()
{
}


void SkyBoxUI::Render_Update()
{
	ComponentTitle("SkyBox");
	CSkyBox* pSkyBox = GetTargetObject()->SkyBox();

	vector<string> vecMode = {"SPHERE", "CUBE"};

	// Mode
	int SkyBoxMode = (int)pSkyBox->GetMode();

	ImGui::Text("Current Mode:");
	ImGui::SameLine();
	// SPHERE
	if (SkyBoxMode == 0)
	{
		ImGui::Text("SPHERE");
	}

	// CUBE
	else
	{
		ImGui::Text("CUBE");
	}

	// 콤보박스 생성
	if (ImGui::BeginCombo("##Select SkyBoxMode", vecMode[SkyBoxMode].c_str()))
	{
		for (int i = 0; i < vecMode.size(); ++i)
		{
			bool isSelected = (SkyBoxMode == i);

			if (ImGui::Selectable(vecMode[i].c_str(), isSelected))
			{
				SkyBoxMode = i;
				pSkyBox->SetMode((SKYBOX_MODE)SkyBoxMode);
			}
			// 선택된 항목 강조
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}


	if (ImGui::Button("DELETE##SkyBox"))
	{
		DeleteComponent(COMPONENT_TYPE::SKYBOX);
	}
}
