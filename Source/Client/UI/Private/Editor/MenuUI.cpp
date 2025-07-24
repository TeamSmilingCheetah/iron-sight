#include "pch.h"
#include "Client/UI/Public/Editor/MenuUI.h"
#include "Client/UI/Public/Editor/Inspector.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CPathMgr.h"
#include "Engine/System/Public/Manager/RenderManager.h"
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
			// 파일 경로 문자열
			wchar_t szFilePath[255] = {};

			OPENFILENAME Desc = {};

			Desc.lStructSize = sizeof(OPENFILENAME);
			Desc.hwndOwner = nullptr;
			Desc.lpstrFile = szFilePath;
			Desc.nMaxFile = 255;
			Desc.lpstrFilter = L"Level\0*.lv\0ALL\0*.*";
			Desc.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
			strFilePath += L"Level";
			Desc.lpstrInitialDir = strFilePath.c_str();



			if (GetSaveFileName(&Desc))
			{
				if (wcslen(szFilePath) != 0)
				{
					CLevel* CurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
					CLevelMgr::SaveLevel(szFilePath, CurLevel);
				}
			}
		}

		if (ImGui::MenuItem("Level Load"))
		{
			wstring m_SelectedFilePath;
			OPENFILENAME ofn;
			wchar_t filePath[MAX_PATH] = L"";

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = filePath;
			ofn.nMaxFile = sizeof(filePath);
			ofn.lpstrFilter = L"Level Files (*.lv)\0*.lv\0All Files (*.*)\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + L"Level";;
			ofn.lpstrInitialDir = strFilePath.c_str();


			// 다이얼로그 취소시 아무 행동 안함 -> EndMenu로
			if (GetOpenFileName(&ofn) && wcslen(filePath) > 0)
			{
				m_SelectedFilePath = filePath;

				CLevel* pLoadedLevel = CLevelMgr::LoadLevel(m_SelectedFilePath);
				ChangeLevel(pLoadedLevel, LEVEL_STATE::STOP);


				// 레벨이 로드될때 Inspector 에서 보여주던 정보를 전부 제거한다. (삭제된 객체를 가리키고 있을 수 있기 때문)
				Inspector* pInspector = (Inspector*)CImGuiMgr::GetInst()->FindUI("Inspector");
				pInspector->SetTargetObject(nullptr);
			}		
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

		if (ImGui::MenuItem("Target Object"))
		{
			CImGuiMgr::GetInst()->FindUI("TargetObject")->SetActive(true);
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

#define TARGET_COUNT 7

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
			FRenderManager::GetInst()->SetSpecifyTarget(pTarget);
		}
		else
		{
			FRenderManager::GetInst()->SetSpecifyTarget(nullptr);
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
