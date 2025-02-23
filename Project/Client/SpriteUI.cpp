#include "pch.h"
#include "SpriteUI.h"

SpriteUI::SpriteUI()
    : AssetUI("Sprite", ASSET_TYPE::SPRITE)
{
}

SpriteUI::~SpriteUI()
{
}

void SpriteUI::Render_Update()
{
    AssetTitle();
}
