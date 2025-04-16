#include "pch.h"
#include "Client/UI/Public/Asset/SpriteUI.h"

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
