#include "pch.h"
#include "Client/UI/Public/Asset/SoundUI.h"

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
