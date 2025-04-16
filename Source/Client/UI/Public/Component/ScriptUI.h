#pragma once
#include "Client/UI/Public/Component/ComponentUI.h"

class CScript;

class ScriptUI :
	public ComponentUI
{
	CScript* m_TargetScript;

public:
	void SetScript(CScript* _Script);

	void Render_Update() override;

	ScriptUI();
	~ScriptUI() override;
};
