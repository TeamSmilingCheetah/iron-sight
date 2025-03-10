#pragma once
#include "Client/UI/Public/Editor/EditorUI.h"

class TreeNode;
class CGameObject;

class Outliner :
	public EditorUI
{
	class TreeUI* m_Tree;
	CGameObject* m_TargetObject;

public:
	void Render_Update() override;

private:
	void RenewGameObject();
	void AddGameObject(TreeNode* _ParentNode, CGameObject* _Object);

	void SelectGameObject(DWORD_PTR _TreeNode);
	void DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode);

	void ChangeName(DWORD_PTR _TreeNode);
	void Prefab(DWORD_PTR _TreeNode);
	void Copy(DWORD_PTR _TreeNode);
	void DeleteObject(DWORD_PTR _TreeNode);

public:
	Outliner();
	~Outliner() override;
};
