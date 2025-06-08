#include "pch.h"
#include "Client/UI/Public/Asset/AnimationUI.h"


AnimationUI::AnimationUI()
	: AssetUI("Animation", ANIMATION)
{
}

AnimationUI::~AnimationUI()
{
}

void AnimationUI::Render_Update()
{
	AssetTitle();

	//ImGui::Button("AssetCopy", ImVec2())

	SaveButton();
}
