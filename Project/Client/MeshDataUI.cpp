#include "pch.h"
#include "MeshDataUI.h"

MeshDataUI::MeshDataUI()
    : AssetUI("MeshData", MESH_DATA)
{
}

MeshDataUI::~MeshDataUI()
{
}

void MeshDataUI::Render_Update()
{
    AssetTitle();
}
