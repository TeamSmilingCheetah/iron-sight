#include "pch.h"
#include "Client/UI/Public/Component/MeshColliderUI.h"
#include "Engine/Runtime/Public/Component/Physics/MeshCollider.h"


MeshColliderUI::MeshColliderUI()
	: ComponentUI("MeshColliderUI", COMPONENT_TYPE::MESH_COLLIDER)
{
}

MeshColliderUI::~MeshColliderUI()
{
}

void MeshColliderUI::Render_Update()
{
	ComponentTitle("MeshCollider");
}
