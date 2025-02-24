#pragma once
#include "ComponentUI.h"

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
