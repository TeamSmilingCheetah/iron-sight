#include "pch.h"
#include "ListUI.h"

#include "imgui/imgui.h"

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
                m_SelectedIdx = i;
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                // ����Ŭ���� �׸� ���ڿ� ���
                m_strSelected = m_vecList[i];

                // ��ϵ� Delegate ȣ��
                if (m_DbclickedInst && m_DbclickedFunc)
                {
                    (m_DbclickedInst->*m_DbclickedFunc)(static_cast<DWORD_PTR>(this),
                                                        (DWORD_PTR) & m_strSelected);
                }


                // ��Ȱ��ȭ
                SetActive(false);
            }

            ImGui::TreePop();
        }
    }
}
