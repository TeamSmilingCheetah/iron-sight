#include "pch.h"
#include "Client/UI/Public/Component/BoxColliderUI.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"

BoxColliderUI::BoxColliderUI()
	: ComponentUI("BoxColliderUI", COMPONENT_TYPE::BOX_COLLIDER)
{
}

BoxColliderUI::~BoxColliderUI() = default;

void BoxColliderUI::Render_Update()
{
	ComponentTitle("BoxCollider");

	FBoxCollider* pBoxCollider = GetTargetObject()->BoxCollider();

	Vec3 vOffset = pBoxCollider->GetOffset();
	Vec3 vScale = pBoxCollider->GetScale();
	float fRotY = pBoxCollider->GetRotY();
	bool bIndependent = pBoxCollider->IsIndependentScale();
	bool bActive = pBoxCollider->IsActive();
	bool bIndependentRot = pBoxCollider->IsIndependentRot();
	bool bTrigger = pBoxCollider->IsTrigger();

	ImGui::Text("Scale");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##ColliderScale", vScale, 0.1f);
	pBoxCollider->SetScale(vScale);

	ImGui::Text("Offset");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##ColliderOffset", vOffset, 0.1f);
	pBoxCollider->SetOffset(vOffset);

	ImGui::Text("Rotation");
	ImGui::SameLine(100);
	ImGui::DragFloat("##ColliderRotation", &fRotY, 0.01f);
	pBoxCollider->SetRotY(fRotY);

	ImGui::Text("Independent");
	ImGui::SameLine(100);
	ImGui::Checkbox("##Independent", &bIndependent);
	pBoxCollider->SetIndependentScale(bIndependent);

	ImGui::Text("IsActive");
	ImGui::SameLine(100);
	if (ImGui::Checkbox("##IsActive", &bActive))
	{
		if (bActive)
		{
			pBoxCollider->SetActivate();
		}
		else
		{
			pBoxCollider->SetSemiDeactivate();
		}
	}

	ImGui::Text("IsIndependentRotation");
	ImGui::SameLine(100);
	ImGui::Checkbox("##IsIndependentRotation", &bIndependentRot);
	pBoxCollider->SetIndependentRot(bIndependentRot);

	ImGui::Text("IsTrigger");
	ImGui::SameLine(100);
	ImGui::Checkbox("##IsTrigger", &bTrigger);
	pBoxCollider->SetTrigger(bTrigger);

	// Penetration Depth (Read Only)
	float PenetrationDepth = pBoxCollider->GetPenetrationDepth();
	ImGui::Text("Penetration");
	ImGui::SameLine(100);
	ImGui::DragFloat("##ColliderRotation", &PenetrationDepth, 0.01f);

	if (ImGui::Button("DELETE##BoxCollider"))
	{
		DeleteComponent(COMPONENT_TYPE::BOX_COLLIDER);
	}
}
