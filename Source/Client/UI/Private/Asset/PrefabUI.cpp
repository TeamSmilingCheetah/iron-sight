#include "pch.h"
#include "Client/UI/Public/Asset/PrefabUI.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"
#include "Engine/System/Public/Manager/CPathMgr.h"

class CPrefab;

PrefabUI::PrefabUI()
	: AssetUI("Prefab", PREFAB)
{
}

PrefabUI::~PrefabUI()
{
}

void PrefabUI::Render_Update()
{
	AssetTitle();

	Ptr<CPrefab> pAsset = dynamic_cast<CPrefab*>(GetAsset().Get());
	assert(pAsset.Get());

	ImGui::Text("Name");
	ImGui::SameLine(100);

	string strKey = WStringToString(pAsset->GetKey());
	ImGui::InputText("##PrefabName", (char*)strKey.c_str(), strKey.length(),
	                 ImGuiInputTextFlags_ReadOnly);

	if (ImGui::Button("Instantiate"))
	{
		CGameObject* pNewObject = pAsset->Instantiate();
		CreateObject(pNewObject, 0, false);
	}

	if (ImGui::Button("Save"))
	{
		wstring strContentPath = CPathMgr::GetInst()->GetContentPath();

		// 파일 경로 문자열
		wchar_t szFilePath[255] = {};

		OPENFILENAME Desc = {};

		Desc.lStructSize = sizeof(OPENFILENAME);
		Desc.hwndOwner = nullptr;
		Desc.lpstrFile = szFilePath;
		Desc.nMaxFile = 255;
		Desc.lpstrFilter = L"Prefab\0*.pref\0ALL\0*.*";
		Desc.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		Desc.lpstrInitialDir = strContentPath.c_str();

		if (GetSaveFileName(&Desc))
		{
			if (wcslen(szFilePath) != 0)
			{
				pAsset->Save(szFilePath);
			}
		}
	}
}
