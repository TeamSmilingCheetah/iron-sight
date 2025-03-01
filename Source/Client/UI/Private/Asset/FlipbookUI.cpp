#include "pch.h"
#include "UI/Public/Asset/FlipbookUI.h"

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
