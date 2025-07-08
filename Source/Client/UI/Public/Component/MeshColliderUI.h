#pragma once
#include "Client/UI/Public/Component/ComponentUI.h"

class MeshColliderUI :
	public ComponentUI
{
public:
	void Render_Update() override;



public:
	MeshColliderUI();
	~MeshColliderUI() override;
};
