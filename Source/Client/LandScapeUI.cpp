#include "pch.h"
#include "LandScapeUI.h"

#include <Engine/CLandScape.h>


LandScapeUI::LandScapeUI()
	: ComponentUI("LandScapeUI", COMPONENT_TYPE::LANDSCAPE)
{
}

LandScapeUI::~LandScapeUI()
{
}


void LandScapeUI::Render_Update()
{
	ComponentTitle("LandScape");
	CLandScape* pLandScape = GetTargetObject()->LandScape();

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

	int LandScpaeMode = (int)pLandScape->GetMode();

	ImGui::Text("Current Mode:");
	ImGui::SameLine();

	if (LandScpaeMode == 0)
	{
		ImGui::Text("None");
	}

	else if (LandScpaeMode == 1)
	{
		ImGui::Text("HeightMap");
	}

	else
	{
		ImGui::Text("Splating");
	}

	// 콤보박스 생성
	if (ImGui::BeginCombo("##Select LandScpaeMode", vecMode[LandScpaeMode].c_str()))
	{
		for (int i = 0; i < vecMode.size(); ++i)
		{
			bool isSelected = (LandScpaeMode == i);

			if (ImGui::Selectable(vecMode[i].c_str(), isSelected))
			{
				LandScpaeMode = i;
				pLandScape->SetMode((LANDSCAPE_MODE)LandScpaeMode);
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
	ImGui::BeginDisabled(LandScpaeMode == 0);
	int BrushSize = pLandScape->GetBrushSize();
	int BrushIdx = pLandScape->GetBrushIdx();
	Vec2 BrushScale = pLandScape->GetBrushScale();

	ImGui::Text("Brush IDX");
	ImGui::SameLine();
	ImGui::InputInt("##BrsuhIdx", &BrushIdx);
	if (BrushSize <= BrushIdx || BrushIdx <= 0)
	{
		BrushIdx = 0;
	}
	pLandScape->SetBrushIdx((UINT)BrushIdx);

	ImGui::Text("Brush Scale");
	ImGui::SameLine();
	ImGui::InputFloat2("##BrsuhScale", BrushScale);
	pLandScape->SetBrushScale(BrushScale);


	// Weight
	int WeightSize = pLandScape->GetWeightSize();
	int WeightIdx = pLandScape->GetWeigtIdx();

	ImGui::Text("Weight IDX");
	ImGui::SameLine();
	ImGui::InputInt("##WeightIdx", &WeightIdx);
	if (WeightSize <= WeightIdx || WeightIdx <= 0)
	{
		WeightIdx = 0;
	}
	pLandScape->SetWeightIdx(WeightIdx);
	ImGui::EndDisabled();

}
