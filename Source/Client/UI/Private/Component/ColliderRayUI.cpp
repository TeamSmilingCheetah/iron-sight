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
	ComponentTitle("Collider3D");

	CColliderRay* pCollider3D = GetTargetObject()->ColliderRay();

	Vec3 vDir = pCollider3D->GetRayDir();
	Vec3 vOffset = pCollider3D->GetOffset();
	float vLength = pCollider3D->GetRayLength();
	bool bActive = pCollider3D->IsActive();

	ImGui::Text("Direction");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##ColliderDirection", vDir,0.01f);
	pCollider3D->SetRayDir(vDir);

	ImGui::Text("Offset");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##ColliderRayOffset", vOffset);
	pCollider3D->SetOffset(vOffset);

	ImGui::Text("Length");
	ImGui::SameLine(100);
	ImGui::DragFloat("##Independent", &vLength);
	pCollider3D->SetRayLength(vLength);

	ImGui::Text("IsActive");
	ImGui::SameLine(100);
	if (ImGui::Checkbox("##IsActiveRay", &bActive))
	{
		if (bActive)
			pCollider3D->Activate();
		else
			pCollider3D->Deactivate();
	}


	if (ImGui::Button("DELETE##ColliderRay"))
	{
		DeleteComponent(COMPONENT_TYPE::COLLIDERRAY);
	}
}
