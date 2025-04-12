#include "pch.h"
#include "Client/UI/Public/Editor/TreeUI.h"

// =========
// TreeNode
// =========
UINT TreeNode::g_GlobalID = 0;

TreeNode::TreeNode()
	: m_Owner(nullptr)
	, m_Parent(nullptr)
	, m_Data(0)
	, m_Frame(false)
	, m_Selected(false)
	, m_FrameListMode(false)
{
	char buff[50] = {};
	sprintf_s(buff, 50, "##%d", g_GlobalID++);
	m_ID = buff;
}

TreeNode::~TreeNode()
{
	DeleteVec(m_vecChild);
}

void TreeNode::Render_Update()
{
	// 기본 옵션
	int Flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	string padding;

	// 추가 옵션
	if (m_vecChild.empty())
		Flag |= ImGuiTreeNodeFlags_Leaf;
	if (m_Frame)
		Flag |= ImGuiTreeNodeFlags_Framed;
	if (m_FrameListMode)
		Flag |= ImGuiTreeNodeFlags_DefaultOpen;
	if (m_Selected)
		Flag |= ImGuiTreeNodeFlags_Selected;

	if (m_Frame && m_vecChild.empty())
		padding = "   ";

	string Name = padding + m_Name + m_ID;

	bool Open = ImGui::TreeNodeEx(Name.c_str(), Flag);

	// 노드를 클릭하면, 선택 상태로 만들어 준다.
	if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		m_Selected = true;
		m_Owner->AddSelectedNode(this);
	}

	if (ImGui::BeginDragDropSource())
	{
		// Payload
		auto pThis = this;
		ImGui::SetDragDropPayload(m_Owner->GetName().c_str(), &pThis, sizeof(TreeNode*));
		ImGui::Text(m_Name.c_str());
		ImGui::EndDragDropSource();

		// TreeUI 에 Drag 등록
		m_Owner->SetDraggedNode(this);
	}

	if (m_Owner->IsSelfDragDrop())
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (ImGui::AcceptDragDropPayload(m_Owner->GetName().c_str()))
			{
				const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
				TreeNode* pDragNode = *static_cast<TreeNode**>(pPayload->Data);

				// TreeUI 에 Drop 노드 등록
				m_Owner->SetDroppedNode(this);
			}

			ImGui::EndDragDropTarget();
		}
	}

	// 우클릭을 감지하고 팝업 요청
	if (m_Owner->IsRightOption())
	{
		string popupID = "NodePopup" + m_ID; // 노드 ID를 포함한 고유 팝업 ID 생성

		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup(popupID.c_str()); // 고유 ID로 팝업 요청
		}

		const vector<EUI_DELEGATE_1>& vecDelegate = m_Owner->GetOptionDelegate();

		if (ImGui::BeginPopup(popupID.c_str())) // 동일한 고유 ID로 팝업 시작
		{
			// 기존 코드와 동일
			for (size_t i = 0; i < vecDelegate.size(); ++i)
			{
				(m_Owner->GetSeletedRightInst()->*vecDelegate[i])((DWORD_PTR)this);
			}

			ImGui::EndPopup();
		}

	}

	if (Open)
	{
		for (size_t i = 0; i < m_vecChild.size(); ++i)
		{
			m_vecChild[i]->Render_Update();
		}

		ImGui::TreePop();
	}
}


// ======
// TreeUI
// ======
TreeUI::TreeUI()
	: EditorUI("TreeUI")
	, m_Root(nullptr)
	, m_ShowRoot(true)
	, m_MultiSelection(false)
	, m_SelfDragDrop(false)
	, m_SelectedInst(nullptr)
	, m_SelectedFunc(nullptr)
	, m_SelfDragDropInst(nullptr)
	, m_SelfDragDropFunc(nullptr)
	, m_DraggedNode(nullptr)
	, m_DroppedNode(nullptr)
	, m_RightOption(false)
	, m_SeletedRightInst(nullptr)
{
}

TreeUI::~TreeUI()
{
	if (nullptr != m_Root)
		delete m_Root;
}

void TreeUI::Render_Update()
{
	if (nullptr == m_Root)
		return;

	// TreeNode Render
	// 루트노드부터 모든 노드들 RenderUpdate
	if (m_ShowRoot)
		m_Root->Render_Update();
	else
	{
		for (size_t i = 0; i < m_Root->m_vecChild.size(); ++i)
		{
			m_Root->m_vecChild[i]->Render_Update();
		}
	}

	// Drag Drop 처리
	// 드래그 노드가 있고, 마우스 왼쪽이 현재 윈도우에서 떨어진 경우
	if ((m_DraggedNode && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) && ImGui::IsWindowHovered())
	{
		// 등록된 dragdrop delegate 가 있다면
		if (m_SelfDragDropInst && m_SelfDragDropFunc)
		{
			(m_SelfDragDropInst->*m_SelfDragDropFunc)((DWORD_PTR)m_DraggedNode,
			                                          (DWORD_PTR)m_DroppedNode);
		}

		m_DraggedNode = nullptr;
		m_DroppedNode = nullptr;
	}

	// 빈 공간 우클릭 처리
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) &&
		!ImGui::IsAnyItemHovered() &&  // 어떤 아이템도 Hover되지 않은 경우
		ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		ImGui::OpenPopup("SpacePopup");

	if (ImGui::BeginPopup("SpacePopup"))
	{
		// 기존 코드와 동일
		for (size_t i = 0; i < m_vecRightSpaceDelegate.size(); ++i)
		{
			(m_SeletedRightInst->*m_vecRightSpaceDelegate[i])((DWORD_PTR)this);
		}

		ImGui::EndPopup();
	}
}

void TreeUI::AddSelectedNode(TreeNode* _Node)
{
	if (m_MultiSelection)
	{
		m_vecSelected.push_back(_Node);
	}
	else
	{
		for (size_t i = 0; i < m_vecSelected.size(); ++i)
		{
			m_vecSelected[i]->m_Selected = false;
		}
		m_vecSelected.clear();
		m_vecSelected.push_back(_Node);
	}

	// 가장 최근에 선택된 노드에 대해서 Delegate 를 호출시킨다.
	if (m_SelectedInst && m_SelectedFunc)
	{
		(m_SelectedInst->*m_SelectedFunc)((DWORD_PTR)m_vecSelected.back());
	}
}

void TreeUI::SetDroppedNode(TreeNode* _Node)
{
	assert(m_DraggedNode);

	m_DroppedNode = _Node;
}

TreeNode* TreeUI::AddItem(TreeNode* _ParentNode, const string& _Name, DWORD_PTR _Data, bool _Frame, bool _Framenotile)
{
	auto pNode = new TreeNode;

	pNode->m_Owner = this;
	pNode->m_Name = _Name;
	pNode->m_Data = _Data;
	pNode->m_Frame = _Frame;
	pNode->m_FrameListMode = _Framenotile;

	// _ParentNode 가 nullptr 인 경우
	if (nullptr == _ParentNode)
	{
		// Tree 에 추가하는 항목이 루트가 될 것이다.
		assert(!m_Root);
		m_Root = pNode;
	}
	else
	{
		_ParentNode->AddChildNode(pNode);
	}

	return pNode;
}
