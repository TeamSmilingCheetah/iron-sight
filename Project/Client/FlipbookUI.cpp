#include "pch.h"
#include "FlipbookUI.h"

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
