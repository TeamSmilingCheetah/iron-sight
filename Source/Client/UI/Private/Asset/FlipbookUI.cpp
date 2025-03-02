#include "pch.h"
#include "Client/UI/Public/Asset/FlipbookUI.h"

FlipbookUI::FlipbookUI()
    : AssetUI("Flipbook", FLIPBOOK)
{
}

FlipbookUI::~FlipbookUI()
{
}

void FlipbookUI::Render_Update()
{
    AssetTitle();
}
