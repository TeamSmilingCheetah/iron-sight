#include "pch.h"
#include "Client/UI/Public/Component/LandscapeUI.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Rendering/Landscape.h"

LandscapeUI::LandscapeUI()
	: ComponentUI("LandscapeUI", COMPONENT_TYPE::LANDSCAPE)
{
}

LandscapeUI::~LandscapeUI()
{
}


void LandscapeUI::Render_Update()
{
	ComponentTitle("LandScape");
	FLandscape* pLandScape = GetTargetObject()->Landscape();

	// 지형 면 개수
	Vec2 Face = pLandScape->GetFace();

	ImGui::Text("Face X, Z:");
	ImGui::SameLine();
	ImGui::DragFloat2("##FrameIdx", Face, 1.0f);
	if (Face.x <= 0.f)
	{
		Face.x = 0.f;
	}
	if (Face.y <= 0.f)
	{
		Face.y = 0.f;
	}
	pLandScape->SetFace((UINT)Face.x, (UINT)Face.y);


	// LandScape Mode
	vector<string> vecMode = { "None", "HeightMap", "Splating" };

	int LandscapeMode = 0; // (int)pLandScape->GetMode();

	ImGui::Text("Current Mode:");
	ImGui::SameLine();

	if (LandscapeMode == 0)
	{
		ImGui::Text("None");
	}

	else if (LandscapeMode == 1)
	{
		ImGui::Text("HeightMap");
	}

	else
	{
		ImGui::Text("Splating");
	}

	// 콤보박스 생성
	if (ImGui::BeginCombo("##Select LandScpaeMode", vecMode[LandscapeMode].c_str()))
	{
		for (int i = 0; i < vecMode.size(); ++i)
		{
			bool isSelected = (LandscapeMode == i);

			if (ImGui::Selectable(vecMode[i].c_str(), isSelected))
			{
				LandscapeMode = i;
				// pLandScape->SetMode((LANDSCAPE_MODE)LandScpaeMode);
			}
			// 선택된 항목 강조
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// Brush
	ImGui::BeginDisabled(LandscapeMode == 0);
	// int BrushSize = pLandScape->GetBrushSize();
	// int BrushIdx = pLandScape->GetBrushIdx();
	// Vec2 BrushScale = pLandScape->GetBrushScale();

	ImGui::Text("Brush IDX");
	ImGui::SameLine();
	// ImGui::InputInt("##BrsuhIdx", &BrushIdx);
	// if (BrushSize <= BrushIdx || BrushIdx <= 0)
	// {
	// 	BrushIdx = 0;
	// }
	// pLandScape->SetBrushIdx((UINT)BrushIdx);

	ImGui::Text("Brush Scale");
	ImGui::SameLine();
	// ImGui::InputFloat2("##BrsuhScale", BrushScale);
	// pLandScape->SetBrushScale(BrushScale);


	// Weight
	// int WeightSize = pLandScape->GetWeightSize();
	// int WeightIdx = pLandScape->GetWeigtIdx();

	ImGui::Text("Weight IDX");
	ImGui::SameLine();
	// ImGui::InputInt("##WeightIdx", &WeightIdx);
	// if (WeightSize <= WeightIdx || WeightIdx <= 0)
	// {
	// 	WeightIdx = 0;
	// }
	// pLandScape->SetWeightIdx(WeightIdx);
	ImGui::EndDisabled();

	if (ImGui::Button("DELETE##LandScape"))
	{
		DeleteComponent(COMPONENT_TYPE::LANDSCAPE);
	}
}
