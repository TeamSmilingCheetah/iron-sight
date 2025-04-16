#include "pch.h"
#include "Client/UI/Public/Asset/MeshUI.h"

MeshUI::MeshUI()
	: AssetUI("Mesh", MESH)
{
}

MeshUI::~MeshUI()
{
}

void MeshUI::Render_Update()
{
	AssetTitle();
}
