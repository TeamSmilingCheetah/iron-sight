#include "pch.h"
#include "SoundUI.h"

SoundUI::SoundUI()
    : AssetUI("Sound", ASSET_TYPE::SOUND)
{
}

SoundUI::~SoundUI()
{
}

void SoundUI::Render_Update()
{
    AssetTitle();
}
