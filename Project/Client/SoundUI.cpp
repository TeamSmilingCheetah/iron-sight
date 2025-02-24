#include "pch.h"
#include "SoundUI.h"

SoundUI::SoundUI()
    : AssetUI("Sound", SOUND)
{
}

SoundUI::~SoundUI()
{
}

void SoundUI::Render_Update()
{
    AssetTitle();
}
