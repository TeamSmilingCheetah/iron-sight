#include "pch.h"
#include "Client/UI/Public/Component/ComponentUI.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

class CComponent;

ComponentUI::ComponentUI(const string& _ID, COMPONENT_TYPE _Type)
	: EditorUI(_ID)
	  , m_TargetObject(nullptr)
	  , m_Type(_Type)
{
}

ComponentUI::~ComponentUI()
{
}

void ComponentUI::SetTargetObject(CGameObject* _Target)
{
	m_TargetObject = _Target;

	if (nullptr == m_TargetObject)
	{
		SetActive(false);
	}
	else
	{
		CComponent* pComponent = m_TargetObject->GetComponent(m_Type);
		if (nullptr == pComponent)
		{
			SetActive(false);
		}
		else
		{
			SetActive(true);
		}
	}
}

void ComponentUI::ComponentTitle(const string& _title)
{
	ImGui::PushID(0);

	ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.f, 0.6f, 0.6f)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
	                      static_cast<ImVec4>(ImColor::HSV(0.f, 0.6f, 0.6f)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,
	                      static_cast<ImVec4>(ImColor::HSV(0.f, 0.6f, 0.6f)));

	ImGui::Button(_title.c_str());

	ImGui::PopStyleColor(3);
	ImGui::PopID();
}
