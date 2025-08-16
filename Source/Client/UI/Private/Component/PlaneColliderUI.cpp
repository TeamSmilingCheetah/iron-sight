#include "pch.h"
#include "Client/UI/Public/Component/PlaneColliderUI.h"
#include "Engine/Runtime/Public/Component/Physics/PlaneCollider.h"

class FPlaneCollider;

PlaneColliderUI::PlaneColliderUI()
	: ComponentUI("PlaneColliderUI", COMPONENT_TYPE::PLANE_COLLIDER)
{
}

PlaneColliderUI::~PlaneColliderUI()
{
}

void PlaneColliderUI::Render_Update()
{
	ComponentTitle("PlaneCollider");

	FPlaneCollider* pPlaneCollider = GetTargetObject()->PlaneCollider();

	Vec2 vOffset = pPlaneCollider->GetOffset();
	Vec2 vScale = pPlaneCollider->GetScale();
	bool bIndependent = pPlaneCollider->IsIndependentScale();

	ImGui::Text("Scale");
	ImGui::SameLine(100);
	ImGui::DragFloat2("##ColliderScale", vScale);

	ImGui::Text("Offset");
	ImGui::SameLine(100);
	ImGui::DragFloat2("##ColliderOffset", vOffset);

	ImGui::Text("Independent");
	ImGui::SameLine(100);
	ImGui::Checkbox("##Independent", &bIndependent);

	if (ImGui::Button("DELETE##PlaneCollider"))
	{
		DeleteComponent(COMPONENT_TYPE::PLANE_COLLIDER);
	}
}
