#pragma once
#include "Client/UI/Public/Editor/EditorUI.h"

class TreeNode;
class CGameObject;

class Outliner :
	public EditorUI
{
private:
	class TreeUI* m_Tree;
	CGameObject* m_TargetObject;

	int             m_ObjectIdx;    // 32개 까지 새 오브젝트

public:
	void Render_Update() override;
	void SelectAndScrollToObject(CGameObject* _TargetObject);

private:
	void RenewGameObject();
	void AddGameObject(TreeNode* _ParentNode, CGameObject* _Object);

	void SelectGameObject(DWORD_PTR _TreeNode);
	void DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode);

	// TargetObject 노드추적용 추가 기능
	TreeNode* FindNodeByGameObject(TreeNode* _StartNode, CGameObject* _TargetObject);
	void ExpandParentNodes(TreeNode* _Node);

	// Outliner 빈 땅에 우클릭 관련 함수
	void CreateObject_Outliner(Ptr<CMesh> _pMesh);
	void CheckDefaultName(const string& _OldName);
	void ClearObjectIdx(int _idx) { m_ObjectIdx &= ~(1 << _idx); }

	// TreeUI item 우클릭 관련 Delegate
	void ChangeName_Outliner(DWORD_PTR _TreeNode);
	void CopyObject(DWORD_PTR _TreeNode);
	void DeleteObject(DWORD_PTR _TreeNode);
	void MoveToObject(DWORD_PTR _TreeNode);
	void MakePrefab(DWORD_PTR _TreeNode);

	// TreeUI space 우클릭 관련 Delegate
	void CreateObject_Outliner(DWORD_PTR _Nothing);

public:
	Outliner();
	~Outliner() override;
};
