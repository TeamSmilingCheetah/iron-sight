#pragma once
#include "EditorUI.h"

class SE_AtlasView;
class SE_Detail;

class SpriteEditor :
    public EditorUI
{
    SE_AtlasView* m_AtlasView;
    SE_Detail* m_Detail;

public:
    SE_AtlasView* GetAtlasView() { return m_AtlasView; }
    SE_Detail* GetDetail() { return m_Detail; }

    void Init();
    void Render_Update() override;

    void Activate() override;
    void Deactivate() override;

    SpriteEditor();
    ~SpriteEditor() override;
};
