#include "pch.h"
#include "FlipbookUI.h"

FlipbookUI::FlipbookUI()
    : AssetUI("Flipbook", ASSET_TYPE::FLIPBOOK)
{
}

FlipbookUI::~FlipbookUI()
{
}

void FlipbookUI::Render_Update()
{
    AssetTitle();
}
