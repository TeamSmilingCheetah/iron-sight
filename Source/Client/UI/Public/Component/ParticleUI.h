#pragma once
#include "ComponentUI.h"

class ParticleUI :
	public ComponentUI
{
private:

public:
	virtual void Render_Update() override;


	void SelectMesh(DWORD_PTR _ListUI, DWORD_PTR _SelectString);
	void SelectMaterial(DWORD_PTR _ListUI, DWORD_PTR _SelectString);

public:
	ParticleUI();
	~ParticleUI();
};

