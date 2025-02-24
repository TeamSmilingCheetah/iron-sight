#pragma once
#include "EditorUI.h"


class MenuUI :
    public EditorUI
{
public:
    void Render() override;

    void Render_Update() override
    {
    };

private:
    void File();
    void Level();
    void GameObject();
    void Editor();
    void Asset();
    void RenderTarget();

    void SelectRenderTarget(const string& _ItemName, const wstring& _TargetKey, bool* pTarget,
                            int _idx);

    wstring GetNextMaterialName();

public:
    MenuUI();
    ~MenuUI() override;
};
