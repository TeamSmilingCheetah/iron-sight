#include "pch.h"
#include "Client/UI/Public/Component/RayColliderUI.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Physics/RayCollider.h"


RayColliderUI::RayColliderUI()
	: ComponentUI("RayColliderUI", COMPONENT_TYPE::RAY_COLLIDER)
{
}

RayColliderUI::~RayColliderUI()
{
}

void RayColliderUI::Render_Update()
{
	ComponentTitle("RayCollider");

	FRayCollider* pRayCollider = GetTargetObject()->RayCollider();

	Vec3 vDir = pRayCollider->GetRayDir();
	Vec3 vOffset = pRayCollider->GetOffset();
	float vLength = pRayCollider->GetRayLength();
	bool bIndependent = pRayCollider->IsIndependentDir();
	bool bActive = pRayCollider->IsActive();

	ImGui::Text("Direction");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##ColliderDirection", vDir,0.01f);
	pRayCollider->SetRayDir(vDir);

	ImGui::Text("Offset");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##RayColliderOffset", vOffset);
	pRayCollider->SetOffset(vOffset);

	ImGui::Text("Length");
	ImGui::SameLine(100);
	ImGui::DragFloat("##Independent", &vLength);
	pRayCollider->SetRayLength(vLength);

	ImGui::Text("Independent");
	ImGui::SameLine(100);
	ImGui::Checkbox("##IndependentRay", &bIndependent);
	pRayCollider->SetIndependentDir(bIndependent);

	ImGui::Text("IsActive");
	ImGui::SameLine(100);
	if (ImGui::Checkbox("##IsActiveRay", &bActive))
	{
		if (bActive)
			pRayCollider->SetActivate();
		else
			pRayCollider->SetSemiDeactivate();
	}


	if (ImGui::Button("DELETE##RayCollider"))
	{
		DeleteComponent(COMPONENT_TYPE::RAY_COLLIDER);
	}
}
