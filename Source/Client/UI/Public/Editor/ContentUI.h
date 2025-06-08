#pragma once
#include "Client/UI/Public/Editor/EditorUI.h"

class ContentUI :
	public EditorUI
{
private:
	class TreeUI*	m_Tree;
	vector<wstring> m_vecAssetPath; // Content 폴더에 있는 모든 리소스 경로

	Ptr<CAsset>		m_TargetAsset;

public:
	void Render_Update() override;
	void Reset();

private:
	void RenewContent();
	void ReloadContent();

	void SelectAsset(DWORD_PTR _TreeNode);

	void FindAssetPath(const wstring& _FolderPath);
	ASSET_TYPE GetAssetType(const wstring& _Path);

	// TreeUI item 우클릭 관련 Delegate
	void ChangeName_ContentUI(DWORD_PTR _TreeNode);
	void CopyAsset(DWORD_PTR _TreeNode);

public:
	ContentUI();
	~ContentUI() override;
};
