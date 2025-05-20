#include "pch.h"
#include "Client/UI/Public/Editor/MenuUI.h"
#include "Client/UI/Public/Editor/Inspector.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CPathMgr.h"
#include "Engine/System/Public/Manager/CRenderMgr.h"
#include "Client/System/Public/CImGuiMgr.h"

class Inspector;
class CMaterial;

MenuUI::MenuUI()
	: EditorUI("MenuUI")
{
}

MenuUI::~MenuUI()
{
}

void MenuUI::Render()
{
	if (!IsActive())
		return;

	if (ImGui::BeginMainMenuBar())
	{
		File();

		Level();

		GameObject();

		Editor();

		Asset();

		RenderTarget();

		ImGui::EndMainMenuBar();
	}
}

void MenuUI::File()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Level Save"))
		{
			wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
			strFilePath += L"Level\\Test.lv";

			CLevel* CurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
			CLevelMgr::SaveLevel(strFilePath, CurLevel);
		}

		if (ImGui::MenuItem("Level Load"))
		{
			wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
			strFilePath += L"Level\\Test.lv";

			CLevel* pLoadedLevel = CLevelMgr::LoadLevel(strFilePath);
			ChangeLevel(pLoadedLevel, LEVEL_STATE::STOP);

			// 레벨이 로드될때 Inspector 에서 보여주던 정보를 전부 제거한다. (삭제된 객체를 가리키고 있을 수 있기 때문)
			auto pInspector = static_cast<Inspector*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
			pInspector->SetTargetObject(nullptr);
		}

		ImGui::EndMenu();
	}
}

void MenuUI::Level()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	auto State = LEVEL_STATE::NONE;

	if (nullptr != pCurLevel)
		State = pCurLevel->GetState();

	bool IsNotPlay = State != LEVEL_STATE::PLAY;
	bool IsPlay = !IsNotPlay;
	bool IsPause = State == LEVEL_STATE::PAUSE;

	if (ImGui::BeginMenu("Level"))
	{
		if (ImGui::MenuItem("Play", nullptr, nullptr, IsNotPlay))
		{
			CLevelMgr::SaveLevel(CPathMgr::GetInst()->GetContentPath() + L"Level\\Temp.lv",
			                          pCurLevel);

			ChangeLevelState(LEVEL_STATE::PLAY);
		}

		if (ImGui::MenuItem("Pause", nullptr, nullptr, IsPlay))
		{
			ChangeLevelState(LEVEL_STATE::PAUSE);
		}

		if (ImGui::MenuItem("Stop", nullptr, nullptr, IsPlay || IsPause))
		{
			// 레벨이 정지될때 Inspector 에서 보여주던 정보를 전부 제거한다. (삭제된 객체를 가리키고 있을 수 있기 때문)
			auto pInspector = static_cast<Inspector*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
			pInspector->SetTargetObject(nullptr);

			CLevel* pLevel = CLevelMgr::LoadLevel(
				CPathMgr::GetInst()->GetContentPath() + L"Level\\Temp.lv");
			ChangeLevel(pLevel, LEVEL_STATE::STOP);
		}

		ImGui::EndMenu();
	}
}

void MenuUI::GameObject()
{
	if (ImGui::BeginMenu("GameObject"))
	{
		if (ImGui::MenuItem("Add Object"))
		{
			CImGuiMgr::GetInst()->FindUI("AddObjectUI")->SetActive(true);

		}
		ImGui::EndMenu();
	}
}

void MenuUI::Editor()
{
	if (ImGui::BeginMenu("Editor"))
	{
		if (ImGui::MenuItem("Edit Prefab"))
		{
			CImGuiMgr::GetInst()->FindUI("PrefabEditor")->SetActive(true);
		}
		ImGui::EndMenu();
	}
}

void MenuUI::Asset()
{
	if (ImGui::BeginMenu("Asset"))
	{
		if (ImGui::MenuItem("Create Empty Material", nullptr, nullptr))
		{
			Ptr<CMaterial> pMtrl = new CMaterial;
			wstring MtrlKey = GetNextMaterialName();
			CAssetMgr::GetInst()->AddAsset(MtrlKey, pMtrl);
		}

		EditorUI* pSpriteEditor = CImGuiMgr::GetInst()->FindUI("SpriteEditor");
		bool IsActive = pSpriteEditor->IsActive();

		if (ImGui::MenuItem("Sprite Editor", nullptr, &IsActive))
		{
			pSpriteEditor->SetActive(IsActive);
		}

		ImGui::EndMenu();
	}
}

#define TARGET_COUNT 8

void MenuUI::RenderTarget()
{
	if (ImGui::BeginMenu("RenderTarget"))
	{
		static bool bTarget[TARGET_COUNT] = {};

		SelectRenderTarget("ColorTarget", L"ColorTargetTex", bTarget, 0);
		SelectRenderTarget("NormalTarget", L"NormalTargetTex", bTarget, 1);
		SelectRenderTarget("PositionTarget", L"PositionTargetTex", bTarget, 2);
		SelectRenderTarget("EmissiveTarget", L"EmissiveTargetTex", bTarget, 3);
		SelectRenderTarget("DataTarget", L"DataTargetTex", bTarget, 4);
		SelectRenderTarget("DiffuseTarget", L"DiffuseTargetTex", bTarget, 5);
		SelectRenderTarget("SpecularTarget", L"SpecularTargetTex", bTarget, 6);
		SelectRenderTarget("ParticleTarget", L"ParticleTargetTex", bTarget, 7);

		ImGui::EndMenu();
	}
}

void MenuUI::SelectRenderTarget(const string& _ItemName, const wstring& _TargetKey, bool* pTarget,
                                int _Idx)
{
	if (ImGui::MenuItem(_ItemName.c_str(), nullptr, pTarget + _Idx))
	{
		if (*(pTarget + _Idx))
		{
			Ptr<CTexture> pTarget = CAssetMgr::GetInst()->FindAsset<CTexture>(_TargetKey);
			CRenderMgr::GetInst()->SetSpecifyTarget(pTarget);
		}
		else
		{
			CRenderMgr::GetInst()->SetSpecifyTarget(nullptr);
		}

		for (int i = 0; i < TARGET_COUNT; ++i)
		{
			if (i == _Idx)
				continue;

			pTarget[i] = false;
		}
	}
}

wstring MenuUI::GetNextMaterialName()
{
	wchar_t Buff[255] = {};

	UINT i = 0;
	while (true)
	{
		wsprintf(Buff, L"Material\\Default Material %d.mtrl", i++);

		if (nullptr == CAssetMgr::GetInst()->FindAsset<CMaterial>(Buff))
			break;
	}

	return Buff;
}
