#include "pch.h"
#include "MeshUI.h"

MeshUI::MeshUI()
    : AssetUI("Mesh", ASSET_TYPE::MESH)
{
}

MeshUI::~MeshUI()
{
}

void MeshUI::Render_Update()
{
    AssetTitle();
}
