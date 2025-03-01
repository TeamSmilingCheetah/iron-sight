#include "pch.h"
#include "UI/Public/Editor/Outliner.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Actor/CLayer.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTaskMgr.h"
#include "System/Public/CImGuiMgr.h"
#include "UI/Public/Editor/Inspector.h"
#include "UI/Public/Editor/TreeUI.h"

class Inspector;
class CLayer;

Outliner::Outliner()
	: EditorUI("Outliner")
{
	m_Tree = static_cast<TreeUI*>(AddChildUI(new TreeUI));
	m_Tree->SetName("Outliner");
	m_Tree->ShowRoot(false);
	m_Tree->SelfDragDrop(true);

	m_Tree->AddDynamicSelect(this, static_cast<EUI_DELEGATE_1>(&Outliner::SelectGameObject));
	m_Tree->AddDynamicSelfDragDrop(this, static_cast<EUI_DELEGATE_2>(&Outliner::DragDrop));
}

Outliner::~Outliner()
{
}

void Outliner::Render_Update()
{
	if (CTaskMgr::GetInst()->IsLevelChanged())
	{
		RenewGameObject();
	}
}

void Outliner::RenewGameObject()
{
	// 새로 만들기 전에 초기화
	m_Tree->Clear();

	// 최상위 부모 노드생성
	TreeNode* pRootNode = m_Tree->AddItem(nullptr, "RootNode", 0);

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	for (size_t i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurLevel->GetLayer(static_cast<int>(i));
		string LayerName = WStringToString(pLayer->GetName());
		TreeNode* pListNode = m_Tree->AddItem(pRootNode, LayerName, (DWORD_PTR)i, false, true);

		const vector<CGameObject*>& vecParents = pLayer->GetParentObjects();
		for (size_t j = 0; j < vecParents.size(); ++j)
		{
			AddGameObject(pListNode, vecParents[j]);
		}
	}
}

void Outliner::AddGameObject(TreeNode* _ParentNode, CGameObject* _Object)
{
	auto Name = WStringToString(_Object->GetName());

	TreeNode* pCurNode = m_Tree->AddItem(_ParentNode, Name, (DWORD_PTR)_Object);

	const vector<CGameObject*>& vecChild = _Object->GetChild();

	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		AddGameObject(pCurNode, vecChild[i]);
	}
}

void Outliner::SelectGameObject(DWORD_PTR _TreeNode)
{
	TreeNode* pNode = (TreeNode*)_TreeNode;
	CGameObject* pTarget = reinterpret_cast<CGameObject*>(pNode->GetData());

	// 100보다 작다면 이건 오브젝트 포인터가 아니다.
	if (100 > (LONGLONG)pTarget)
		return;

	Inspector* pInspector = (Inspector*)CImGuiMgr::GetInst()->FindUI("Inspector");
	pInspector->SetTargetObject(pTarget);
}

void Outliner::DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode)
{
	TreeNode* pDragged = (TreeNode*)_DragNode;
	TreeNode* pDropped = (TreeNode*)_DropNode;

	// 드래그된 노드의 데이터 가져오기
	DWORD_PTR DragData = pDragged->GetData();

	// 드래그된 항목이 레이어 노드인지 먼저 확인 (값이 1000보다 작으면 레이어 노드)
	if (reinterpret_cast<void*>(DragData) <= (void*)1000)
	{
		// 레이어 노드는 드래그 앤 드롭 대상이 아님
		return;
	}

	CGameObject* pDragObj = reinterpret_cast<CGameObject*>(pDragged->GetData());
	CGameObject* pDropObj = nullptr;

	if (pDragged && !pDragObj)
	{
		// 드래그된 노드가 CGameObject가 아닌 경우 return
		return;
	}

	if (pDropped)
	{
		// pDropped의 데이터를 DWORD_PTR에서 CGameObject*로 변환
		DWORD_PTR DropData = pDropped->GetData();

		// DropData가 CGameObject*인지 long long인지 구별하기 위한 확인
		// 1000 보다 크면 포인터로 본다.
		if (reinterpret_cast<void*>(DropData) > (void*)100)
		{
			pDropObj = reinterpret_cast<CGameObject*>(DropData);

			if (pDropObj->IsAncestor(pDragObj))
				return;
		}
		// DropData가 long long이라면
		else
		{
			int LayerIdx = static_cast<int>(DropData);

			tTask ptask;
			ptask.Type = TASK_TYPE::CHANGE_LAYEROBJECT;
			ptask.Param0 = reinterpret_cast<DWORD_PTR>(pDragObj);
			ptask.Param1 = (DWORD_PTR)LayerIdx;
			CTaskMgr::GetInst()->AddTask(ptask);
		}
	}

	AddChild(pDropObj, pDragObj);
}
