#include "pch.h"
#include "UI/Public/Asset/SoundUI.h"

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
