#include "pch.h"
#include "PrefabUI.h"

#include <Engine/CPrefab.h>
#include <Engine/CPathMgr.h>

PrefabUI::PrefabUI()
    : AssetUI("Prefab", ASSET_TYPE::PREFAB)
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

    string strKey = string(pAsset->GetKey().begin(), pAsset->GetKey().end());
    ImGui::InputText("##PrefabName", static_cast<char*>(strKey.c_str()), strKey.length(),
                     ImGuiInputTextFlags_ReadOnly);

    if (ImGui::Button("Instantiate"))
    {
        CGameObject* pNewObject = pAsset->Instantiate();
        CreateObject(pNewObject, 0, false);
    }

    if (ImGui::Button("Save"))
    {
        wstring strContentPath = CPathMgr::GetInst()->GetContentPath();

        // ���� ��� ���ڿ�
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
