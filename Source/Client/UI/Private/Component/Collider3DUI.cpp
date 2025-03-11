#include "pch.h"
#include "Client/UI/Public/Component/Collider3DUI.h"

#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"


Collider3DUI::Collider3DUI()
	: ComponentUI("Collider3DUI", COMPONENT_TYPE::COLLIDER3D)
{
}

Collider3DUI::~Collider3DUI()
{
}

void Collider3DUI::Render_Update()
{
	ComponentTitle("Collider3D");

	CCollider3D* pCollider3D = GetTargetObject()->Collider3D();

	Vec3 vOffset = pCollider3D->GetOffset();
	Vec3 vScale = pCollider3D->GetScale();
	bool bIndependent = pCollider3D->IsIndependentScale();
	bool bActive = pCollider3D->IsActive();

	ImGui::Text("Scale");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##ColliderScale", vScale);
	pCollider3D->SetScale(vScale);

	ImGui::Text("Offset");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##ColliderOffset", vOffset);
	pCollider3D->SetOffset(vOffset);

	ImGui::Text("Independent");
	ImGui::SameLine(100);
	ImGui::Checkbox("##Independent", &bIndependent);
	pCollider3D->SetIndependentScale(bIndependent);

	ImGui::Text("IsActive");
	ImGui::SameLine(100);
	if (ImGui::Checkbox("##IsActive", &bActive))
	{
		if(bActive)
			pCollider3D->Activate();
		else
			pCollider3D->Deactivate();
	}


	if (ImGui::Button("DELETE##Collider3D"))
	{
		DeleteComponent(COMPONENT_TYPE::COLLIDER3D);
	}

}
