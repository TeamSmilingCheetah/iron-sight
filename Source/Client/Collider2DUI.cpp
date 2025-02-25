#include "pch.h"
#include "Collider2DUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CCollider2D.h>

Collider2DUI::Collider2DUI()
    : ComponentUI("Collider2DUI", COMPONENT_TYPE::COLLIDER2D)
{
}

Collider2DUI::~Collider2DUI()
{
}

void Collider2DUI::Render_Update()
{
    ComponentTitle("Collider2D");

    CCollider2D* pCollider2D = GetTargetObject()->Collider2D();

    Vec2 vOffset = pCollider2D->GetOffset();
    Vec2 vScale = pCollider2D->GetScale();
    bool bIndependent = pCollider2D->IsIndependentScale();

    ImGui::Text("Scale");
    ImGui::SameLine(100);
    ImGui::DragFloat2("##ColliderScale", vScale);

    ImGui::Text("Offset");
    ImGui::SameLine(100);
    ImGui::DragFloat2("##ColliderOffset", vOffset);

    ImGui::Text("Independent");
    ImGui::SameLine(100);
    ImGui::Checkbox("##Independent", &bIndependent);
}
