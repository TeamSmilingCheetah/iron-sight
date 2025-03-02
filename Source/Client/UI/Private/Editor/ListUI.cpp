#include "pch.h"
#include "Client/UI/Public/Editor/ListUI.h"
#include "Client/imgui/imgui.h"

ListUI::ListUI()
	: EditorUI("##ListUI")
	  , m_SelectedIdx(-1)
	  , m_DbclickedInst(nullptr)
	  , m_DbclickedFunc(nullptr)
{
}

ListUI::~ListUI()
{
}

void ListUI::Render_Update()
{
	for (size_t i = 0; i < m_vecList.size(); ++i)
	{
		int Flags = ImGuiTreeNodeFlags_Leaf;

		if (m_SelectedIdx == i)
		{
			Flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (ImGui::TreeNodeEx(m_vecList[i].c_str(), Flags))
		{
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_SelectedIdx = static_cast<int>(i);
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				// 더블클릭한 항목 문자열 기록
				m_strSelected = m_vecList[i];

				// 등록된 Delegate 호출
				if (m_DbclickedInst && m_DbclickedFunc)
				{
					(m_DbclickedInst->*m_DbclickedFunc)((DWORD_PTR)this, (DWORD_PTR)&m_strSelected);
				}

				// 비활성화
				SetActive(false);
			}

			ImGui::TreePop();
		}
	}
}
