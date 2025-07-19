#include "Client/UI/Public/Component/TransformUI.h"
#include "pch.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

class CTransform;

TransformUI::TransformUI()
	: ComponentUI("TransformUI", COMPONENT_TYPE::TRANSFORM)
{
}

TransformUI::~TransformUI()
{
}

void TransformUI::Render_Update()
{
	ComponentTitle("Transform");

	CTransform* pTransform = GetTargetObject()->Transform();

	ImGui::BeginDisabled(pTransform->IsManualUpdate());
	{
		Vec3 vPos = pTransform->GetRelativePos();
		Vec3 vScale = pTransform->GetRelativeScale();
		Vec3 vRot = pTransform->GetRelativeRotation();

		ImGui::Text("Position");
		ImGui::SameLine(100);
		if (ImGui::DragFloat3("##Pos", vPos))
		{
			pTransform->SetRelativePos(vPos);
		}

		ImGui::Text("Scale");
		ImGui::SameLine(100);
		if (ImGui::DragFloat3("##Scale", vScale))
		{
			pTransform->SetRelativeScale(vScale);
		}

		ImGui::Text("Rotation");
		ImGui::SameLine(100);

		if (ImGui::DragFloat3("##Rotation", vRot, 0.1f))
		{
			pTransform->SetRelativeRotation(vRot);
		}
	}
	ImGui::EndDisabled();

	ImGui::Text("Independent Scale");
	ImGui::SameLine(140);
	bool IndependentScale = pTransform->IsIndependentScale();
	if(ImGui::Checkbox("##IndependentScale", &IndependentScale))
	{
		pTransform->SetIndependentScale(IndependentScale);
	}

	ImGui::Text("World Transform Matrix");
	Matrix matTrans = pTransform->GetWorldMat();
	float row[4]{};
	row[0] = matTrans._11; row[1] = matTrans._12; row[2] = matTrans._13; row[3] = matTrans._14;
	ImGui::InputFloat4("##MatWorldTrans1", row, "%.3f", ImGuiInputTextFlags_ReadOnly);
	row[0] = matTrans._21; row[1] = matTrans._22; row[2] = matTrans._23; row[3] = matTrans._24;
	ImGui::InputFloat4("##MatWorldTrans2", row, "%.3f", ImGuiInputTextFlags_ReadOnly);
	row[0] = matTrans._31; row[1] = matTrans._32; row[2] = matTrans._33; row[3] = matTrans._34;
	ImGui::InputFloat4("##MatWorldTrans3", row, "%.3f", ImGuiInputTextFlags_ReadOnly);
	row[0] = matTrans._41; row[1] = matTrans._42; row[2] = matTrans._43; row[3] = matTrans._44;
	ImGui::InputFloat4("##MatWorldTrans4", row, "%.3f", ImGuiInputTextFlags_ReadOnly);

}
