#include "pch.h"
#include "MeshUI.h"

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
