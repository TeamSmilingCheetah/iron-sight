#include "pch.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Client/System/Public/CImGuiMgr.h"
#include "Client/UI/Public/SpriteEditor/SpriteEditor.h"
#include "Client/UI/Public/SpriteEditor/SE_AtlasView.h"
#include "Client/UI/Public/SpriteEditor/SE_Detail.h"

SpriteEditor::SpriteEditor()
	: EditorUI("SpriteEditor")
	  , m_AtlasView(nullptr)
	  , m_Detail(nullptr)
{
	UseMenuBar(true);
}

SpriteEditor::~SpriteEditor()
{
}


void SpriteEditor::Init()
{
	m_AtlasView = static_cast<SE_AtlasView*>(CImGuiMgr::GetInst()->FindUI("SE_AtlasView"));
	m_Detail = static_cast<SE_Detail*>(CImGuiMgr::GetInst()->FindUI("SE_Detail"));

	m_AtlasView->SetMove(false);
	m_Detail->SetMove(false);

	m_AtlasView->m_Owner = this;
	m_Detail->m_Owner = this;

	m_Detail->SetAtlasTex(
		CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\link.png", L"Texture\\link.png"));
}

void SpriteEditor::Render_Update()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Window"))
		{
			bool AtlasView = m_AtlasView->IsActive();
			bool Detail = m_Detail->IsActive();

			if (ImGui::MenuItem("AtlasView", nullptr, &AtlasView))
			{
				m_AtlasView->SetActive(AtlasView);
			}

			if (ImGui::MenuItem("Detail", nullptr, &Detail))
			{
				m_Detail->SetActive(Detail);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void SpriteEditor::Activate()
{
}

void SpriteEditor::Deactivate()
{
}
