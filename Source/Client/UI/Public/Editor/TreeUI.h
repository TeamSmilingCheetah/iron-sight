#pragma once
#include "Client/UI/Public/Editor/EditorUI.h"

class TreeNode
{
	friend class TreeUI;

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
};

class TreeUI :
	public EditorUI
{
private:
	TreeNode*					m_Root;
	vector<TreeNode*>			m_vecSelected;
	bool						m_ShowRoot;
	bool						m_MultiSelection;
	bool						m_SelfDragDrop;

	EditorUI*					m_SelectedInst;
	EUI_DELEGATE_1				m_SelectedFunc;

	EditorUI*					m_SelfDragDropInst;
	EUI_DELEGATE_2				m_SelfDragDropFunc;

	TreeNode*					m_DraggedNode;
	TreeNode*					m_DroppedNode;

	// 우클릭 사용 여부
	bool                        m_RightOption;

	// 아이템 우클릭 옵션용
	EditorUI*					m_SeletedRightInst;
	vector<EUI_DELEGATE_1>      m_vecRightItemDelegate;

	// 빈 공간 우클릭 옵션용
	vector<EUI_DELEGATE_1>		m_vecRightSpaceDelegate;

public:
	void ShowRoot(bool _Show) { m_ShowRoot = _Show; }
	void MutiSelection(bool _Multi) { m_MultiSelection = _Multi; }
	void SelfDragDrop(bool _Self) { m_SelfDragDrop = _Self; }
	bool IsSelfDragDrop() const { return m_SelfDragDrop; }
	bool IsRightOption() const { return m_RightOption; }
	const vector<EUI_DELEGATE_1>& GetOptionDelegate() { return m_vecRightItemDelegate; }
	EditorUI* GetSeletedRightInst() { return m_SeletedRightInst; }

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

	void RightOption(bool _Right, EditorUI* _Inst) { m_RightOption = _Right; m_SeletedRightInst = _Inst; }
	void AddRightItemDelegate(EUI_DELEGATE_1 _Memfunc) { m_vecRightItemDelegate.push_back(_Memfunc); }
	void AddRightSpaceDelegate(EUI_DELEGATE_1 _MemFunc) { m_vecRightSpaceDelegate.push_back(_MemFunc); }

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
