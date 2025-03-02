#pragma once
#include "Client/UI/Public/Editor/EditorUI.h"

class TreeNode
{
private:
	static UINT g_GlobalID;

	class TreeUI* m_Owner;
	TreeNode* m_Parent;
	vector<TreeNode*> m_vecChild;

	string m_Name;
	string m_ID;
	DWORD_PTR m_Data;

	bool m_Frame;
	bool m_FrameListMode;
	bool m_Selected;

public:
	DWORD_PTR GetData() { return m_Data; }

	void Render_Update();

	void AddChildNode(TreeNode* _ChildNode)
	{
		m_vecChild.push_back(_ChildNode);
		_ChildNode->m_Parent = this;
	}

	TreeNode();
	~TreeNode();

	friend class TreeUI;
};

class TreeUI :
	public EditorUI
{
	TreeNode* m_Root;
	vector<TreeNode*> m_vecSelected;
	bool m_ShowRoot;
	bool m_MultiSelection;
	bool m_SelfDragDrop;

	EditorUI* m_SelectedInst;
	EUI_DELEGATE_1 m_SelectedFunc;

	EditorUI* m_SelfDragDropInst;
	EUI_DELEGATE_2 m_SelfDragDropFunc;

	TreeNode* m_DraggedNode;
	TreeNode* m_DroppedNode;

public:
	void ShowRoot(bool _Show) { m_ShowRoot = _Show; }
	void MutiSelection(bool _Multi) { m_MultiSelection = _Multi; }
	void SelfDragDrop(bool _Self) { m_SelfDragDrop = _Self; }
	bool IsSelfDragDrop() { return m_SelfDragDrop; }

	void AddSelectedNode(TreeNode* _Node);
	void SetDraggedNode(TreeNode* _Node) { m_DraggedNode = _Node; }
	void SetDroppedNode(TreeNode* _Node);

	void AddDynamicSelect(EditorUI* _Inst, EUI_DELEGATE_1 _MemFunc)
	{
		m_SelectedInst = _Inst;
		m_SelectedFunc = _MemFunc;
	}

	void AddDynamicSelfDragDrop(EditorUI* _Inst, EUI_DELEGATE_2 _MemFunc)
	{
		m_SelfDragDropInst = _Inst;
		m_SelfDragDropFunc = _MemFunc;
	}

	TreeNode* AddItem(TreeNode* _ParentNode, const string& _Name, DWORD_PTR _Data, bool _Frame = false,
	                  bool _Framenotile = false);

	void Clear()
	{
		if (nullptr != m_Root)
			delete m_Root;
		m_Root = nullptr;
	}

	void Render_Update() override;

	TreeUI();
	~TreeUI() override;
};
