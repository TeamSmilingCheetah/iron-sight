#pragma once
#include "AssetUI.h"

class AnimationUI :
	public AssetUI
{
public:
	virtual void Render_Update() override;

public:
	AnimationUI();
	~AnimationUI();
};
