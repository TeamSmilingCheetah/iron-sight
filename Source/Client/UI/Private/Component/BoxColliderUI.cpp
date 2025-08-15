#include "pch.h"
#include "Client/UI/Public/Component/BoxColliderUI.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"

Collider3DUI::Collider3DUI()
	: ComponentUI("Collider3DUI", COMPONENT_TYPE::BOX_COLLIDER)
{
}

Collider3DUI::~Collider3DUI() = default;

void Collider3DUI::Render_Update()
{
	ComponentTitle("Collider3D");

	FBoxCollider* pCollider3D = GetTargetObject()->BoxCollider();

	Vec3 vOffset = pCollider3D->GetOffset();
	Vec3 vScale = pCollider3D->GetScale();
	float fRotY = pCollider3D->GetRotY();
	bool bIndependent = pCollider3D->IsIndependentScale();
	bool bActive = pCollider3D->IsActive();
	bool bIndependentRot = pCollider3D->IsIndependentRot();
	bool bTrigger = pCollider3D->IsTrigger();

	ImGui::Text("Scale");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##ColliderScale", vScale, 0.1f);
	pCollider3D->SetScale(vScale);

	ImGui::Text("Offset");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##ColliderOffset", vOffset, 0.1f);
	pCollider3D->SetOffset(vOffset);

	ImGui::Text("Rotation");
	ImGui::SameLine(100);
	ImGui::DragFloat("##ColliderRotation", &fRotY, 0.01f);
	pCollider3D->SetRotY(fRotY);

	ImGui::Text("Independent");
	ImGui::SameLine(100);
	ImGui::Checkbox("##Independent", &bIndependent);
	pCollider3D->SetIndependentScale(bIndependent);

	ImGui::Text("IsActive");
	ImGui::SameLine(100);
	if (ImGui::Checkbox("##IsActive", &bActive))
	{
		if (bActive)
		{
			pCollider3D->SetActivate();
		}
		else
		{
			pCollider3D->SetSemiDeactivate();
		}
	}

	ImGui::Text("IsIndependentRotation");
	ImGui::SameLine(100);
	ImGui::Checkbox("##IsIndependentRotation", &bIndependentRot);
	pCollider3D->SetIndependentRot(bIndependentRot);

	ImGui::Text("IsTrigger");
	ImGui::SameLine(100);
	ImGui::Checkbox("##IsTrigger", &bTrigger);
	pCollider3D->SetTrigger(bTrigger);

	// Penetration Depth (Read Only)
	float PenetrationDepth = pCollider3D->GetPenetrationDepth();
	ImGui::Text("Penetration");
	ImGui::SameLine(100);
	ImGui::DragFloat("##ColliderRotation", &PenetrationDepth, 0.01f);

	if (ImGui::Button("DELETE##Collider3D"))
	{
		DeleteComponent(COMPONENT_TYPE::BOX_COLLIDER);
	}
}
