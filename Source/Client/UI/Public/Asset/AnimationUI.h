#pragma once
#include "AssetUI.h"

class CGameObject;

class AnimationUI :
	public AssetUI
{
private:
	CGameObject*	m_SkinnedModel;
	int				m_FrameRange[2];


public:
	virtual void Render_Update() override;

public:
	AnimationUI();
	~AnimationUI();
};
