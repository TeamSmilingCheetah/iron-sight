#include "pch.h"
#include "SpriteUI.h"

SpriteUI::SpriteUI()
    : AssetUI("Sprite", SPRITE)
{
}

SpriteUI::~SpriteUI()
{
}

void SpriteUI::Render_Update()
{
    AssetTitle();
}
