#pragma once
#include "EditorUI.h"

class ContentUI :
    public EditorUI
{
    class TreeUI* m_Tree;
    vector<wstring> m_vecAssetPath; // Content ������ �ִ� ��� ���ҽ� ��� 

public:
    void Render_Update() override;
    void Reset();

private:
    void RenewContent();
    void ReloadContent();

    void SelectAsset(DWORD_PTR _TreeNode);

    void FindAssetPath(const wstring& _FolderPath);
    ASSET_TYPE GetAssetType(const wstring& _Path);

public:
    ContentUI();
    ~ContentUI() override;
};
