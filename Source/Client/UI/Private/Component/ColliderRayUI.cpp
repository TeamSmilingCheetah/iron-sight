#include "pch.h"
#include "Client/UI/Public/Component/ColliderRayUI.h"

#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"


ColliderRayUI::ColliderRayUI()
	: ComponentUI("ColliderRayUI", COMPONENT_TYPE::COLLIDERRAY)
{
}

ColliderRayUI::~ColliderRayUI()
{
}

void ColliderRayUI::Render_Update()
{
	ComponentTitle("ColliderRay");

	CColliderRay* pColliderRay = GetTargetObject()->ColliderRay();

	Vec3 vDir = pColliderRay->GetRayDir();
	Vec3 vOffset = pColliderRay->GetOffset();
	float vLength = pColliderRay->GetRayLength();
	bool bIndependent = pColliderRay->IsIndependentDir();
	bool bActive = pColliderRay->IsActive();

	ImGui::Text("Direction");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##ColliderDirection", vDir,0.01f);
	pColliderRay->SetRayDir(vDir);

	ImGui::Text("Offset");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##ColliderRayOffset", vOffset);
	pColliderRay->SetOffset(vOffset);

	ImGui::Text("Length");
	ImGui::SameLine(100);
	ImGui::DragFloat("##Independent", &vLength);
	pColliderRay->SetRayLength(vLength);

	ImGui::Text("Independent");
	ImGui::SameLine(100);
	ImGui::Checkbox("##IndependentRay", &bIndependent);
	pColliderRay->SetIndependentDir(bIndependent);

	ImGui::Text("IsActive");
	ImGui::SameLine(100);
	if (ImGui::Checkbox("##IsActiveRay", &bActive))
	{
		if (bActive)
			pColliderRay->SetActivate();
		else
			pColliderRay->SetDeactivate();
	}


	if (ImGui::Button("DELETE##ColliderRay"))
	{
		DeleteComponent(COMPONENT_TYPE::COLLIDERRAY);
	}
}
