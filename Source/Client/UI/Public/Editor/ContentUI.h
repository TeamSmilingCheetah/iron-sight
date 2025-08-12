#pragma once
#include "Client/UI/Public/Editor/EditorUI.h"

struct tFSNode	// File System Node
{
	wstring				Name;
	bool				isFolder;
	tFSNode*			Parent;
	vector<tFSNode*>	vecChildren;
	Ptr<FAsset>			Asset;

	tFSNode() : isFolder(false), Parent(nullptr) {}
	tFSNode(const wstring& _Name) : Name(_Name), isFolder(false), Parent(nullptr) {}

	~tFSNode()
	{
		for (tFSNode* child : vecChildren)
		{
			delete child;
		}
	}
};

class ContentUI :
	public EditorUI
{
private:
	class TreeUI*	m_Tree;

	// TEST : 파일 탐색기
	static tFSNode*		m_rootAssetFileSystem;	// dummy root

	Ptr<FAsset>		m_TargetAsset;

public:
	void Render_Update() override;
	void Reset();

	static Ptr<FAsset> LoadAsset(tFSNode* _FSNode);

private:
	void RenewContent();
	void ReloadContent();

	// TEST : 파일 탐색기
	void ConstructFileSystem(tFSNode* _CurFSNode, class TreeNode* _CurTreeNode);

	void SelectAsset(DWORD_PTR _TreeNode);

	void FindAssetPath(const wstring& _FolderPath, tFSNode* _ParentNode);
	static ASSET_TYPE GetAssetType(const wstring& _Path);

	// TreeUI item 우클릭 관련 Delegate
	void ChangeName_ContentUI(DWORD_PTR _TreeNode);
	void CopyAsset(DWORD_PTR _TreeNode);

public:
	ContentUI();
	~ContentUI() override;
};
