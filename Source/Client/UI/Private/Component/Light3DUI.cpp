#include "pch.h"
#include "Client/UI/Public/Component/Light3DUI.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Light/CLight3D.h"

Light3DUI::Light3DUI()
	: ComponentUI("Light3DUI", COMPONENT_TYPE::LIGHT3D)
{
}

Light3DUI::~Light3DUI()
{
}


void Light3DUI::Render_Update()
{
	ComponentTitle("Light3D");
	CLight3D* pLight3D = GetTargetObject()->Light3D();

	tLight3DInfo Light3DInfo = pLight3D->GetLight3DInfo();

	// Color
	Vec3 Color = Light3DInfo.info.vColor;
	ImGui::Text("Color widget:");
	ImGui::SameLine();
	ImGui::ColorPicker4("MyColor##1", (float*)&Color); // RGB 값 편집
	pLight3D->SetLightColor(Color);

	// SpecCoeff
	float SpecCoeff = Light3DInfo.info.SpecCoeff;
	ImGui::Text("SpecCoeff");
	ImGui::SameLine();
	ImGui::InputFloat("##SpecCoeff", &SpecCoeff); // RGB 값 편집
	pLight3D->SetSpecularCoefficient(SpecCoeff);

	// Ambient
	Vec3 Ambient = Light3DInfo.info.vAmbient;
	ImGui::Text("Ambient");
	ImGui::SameLine();
	ImGui::InputFloat3("##Ambient", Ambient); // RGB 값 편집
	pLight3D->SetAmbient(Ambient);

	// LightType
	const char* szLightType[3] = {"Directional", "Point", "Spot"};
	int LightType = (int)Light3DInfo.Type;
	ImGui::Combo("##combo", &LightType, szLightType, 3);
	if (LightType != (int)Light3DInfo.Type)
		pLight3D->SetLightType((LIGHT_TYPE)LightType);

	// Radius
	ImGui::BeginDisabled(LightType == (int)LIGHT_TYPE::DIRECTIONAL);
	float Radius = Light3DInfo.Radius;
	ImGui::Text("Radius");
	ImGui::SameLine(120);
	ImGui::InputFloat("##Radius", &Radius);
	pLight3D->SetRadius(Radius);
	ImGui::EndDisabled();

	// Angle
	ImGui::BeginDisabled(LightType == (int)LIGHT_TYPE::DIRECTIONAL || LightType == (int)LIGHT_TYPE::POINT);
	float Angle = Light3DInfo.Angle;
	ImGui::Text("Angle");
	ImGui::SameLine(120);
	ImGui::InputFloat("##Angle", &Angle);
	pLight3D->SetAngle(Angle);
	ImGui::EndDisabled();


	if (ImGui::Button("DELETE##Light3D"))
	{
		DeleteComponent(COMPONENT_TYPE::LIGHT3D);
	}
}
