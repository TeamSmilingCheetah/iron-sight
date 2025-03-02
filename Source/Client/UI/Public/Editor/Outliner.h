#pragma once
#include "Client/UI/Public/Editor/EditorUI.h"

class TreeNode;
class CGameObject;

class Outliner :
	public EditorUI
{
	class TreeUI* m_Tree;

public:
	void Render_Update() override;

private:
	void RenewGameObject();
	void AddGameObject(TreeNode* _ParentNode, CGameObject* _Object);

	void SelectGameObject(DWORD_PTR _TreeNode);
	void DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode);

public:
	Outliner();
	~Outliner() override;
};
