#include "pch.h"
#include "Outliner.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/CTaskMgr.h>

#include "CImGuiMgr.h"
#include "Inspector.h"
#include "TreeUI.h"


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
    m_Tree->Clear();
    TreeNode* pRootNode = m_Tree->AddItem(nullptr, "RootNode", 0);

    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        CLayer* pLayer = pCurLevel->GetLayer(i);

        const vector<CGameObject*>& vecParents = pLayer->GetParentObjects();
        for (size_t j = 0; j < vecParents.size(); ++j)
        {
            AddGameObject(pRootNode, vecParents[j]);
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
    auto pNode = (TreeNode*)_TreeNode;
    auto pTarget = (CGameObject*)pNode->GetData();

    auto pInspector = static_cast<Inspector*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
    pInspector->SetTargetObject(pTarget);
}

void Outliner::DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode)
{
    auto pDragged = (TreeNode*)_DragNode;
    auto pDropped = (TreeNode*)_DropNode;

    auto pDragObj = (CGameObject*)pDragged->GetData();
    CGameObject* pDropObj = nullptr;

    if (pDropped)
    {
        pDropObj = (CGameObject*)pDropped->GetData();

        // 드래그 된 오브젝트가 목적지 오브젝트의 조상 중 하나라면, 그 상황을 방지한다.
        if (pDropObj->IsAncestor(pDragObj))
            return;
    }

    AddChild(pDropObj, pDragObj);
}
