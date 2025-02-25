#include "pch.h"
#include "SE_Detail.h"

#include "SE_AtlasView.h"
#include "TreeUI.h"
#include "ListUI.h"
#include "CImGuiMgr.h"

#include <Engine/CAssetMgr.h>

SE_Detail::SE_Detail()
    : SE_Sub("SE_Detail")
{
}

SE_Detail::~SE_Detail()
{
}

void SE_Detail::SetAtlasTex(Ptr<CTexture> _Tex)
{
    m_AtlasTex = _Tex;

    GetAtlasView()->SetAtlasTex(m_AtlasTex);
}

void SE_Detail::Render_Update()
{
    Atlas();

    AtlasInfo();
}

void SE_Detail::Atlas()
{
    string TexName;

    if (nullptr != m_AtlasTex)
        TexName = WStringToString(m_AtlasTex->GetKey());

    ImGui::Text("Atlas Texture");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150.f);
    ImGui::InputText("##AtlasTex", (char*)TexName.c_str(),
                     ImGuiInputTextFlags_ReadOnly);

    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentUI");
        if (payload)
        {
            const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
            TreeNode* pNode = *static_cast<TreeNode**>(pPayload->Data);
            Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();

            if (pAsset->GetAssetType() == TEXTURE)
            {
                SetAtlasTex(static_cast<CTexture*>(pAsset.Get()));
            }
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();
    if (ImGui::Button("##AtlasTexBtn", ImVec2(18.f, 18.f)))
    {
        auto pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
        pListUI->SetName("Texture");
        vector<wstring> vecTexNames;
        CAssetMgr::GetInst()->GetAssetNames(TEXTURE, vecTexNames);
        pListUI->AddItem(vecTexNames);
        pListUI->AddDynamicDoubleCliked(
            this, static_cast<EUI_DELEGATE_2>(&SE_Detail::SelectTexture));
        pListUI->SetActive(true);
    }
}

void SE_Detail::AtlasInfo()
{
    // 해상도
    UINT width = 0;
    UINT height = 0;

    if (nullptr != m_AtlasTex)
    {
        width = m_AtlasTex->GetWidth();
        height = m_AtlasTex->GetHeight();
    }

    char buff[50] = {};
    sprintf_s(buff, "%d", width);

    ImGui::Text("Width");
    ImGui::SameLine(100);
    ImGui::InputText("##TextureWidth", buff, 50, ImGuiInputTextFlags_ReadOnly);

    sprintf_s(buff, "%d", height);
    ImGui::Text("Height");
    ImGui::SameLine(100);
    ImGui::InputText("##TextureHeight", buff, 50, ImGuiInputTextFlags_ReadOnly);
}

void SE_Detail::SelectTexture(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
    // 리스트에서 더블킬릭한 항목의 이름을 받아온다.
    auto pListUI = (ListUI*)_ListUI;
    auto pStr = (string*)_SelectString;

    if (*pStr == "None")
    {
        SetAtlasTex(nullptr);
        return;
    }

    // 해당 항목 에셋을 찾아서, MeshRenderComponent 가 해당 메시를 참조하게 한다.
    Ptr<CTexture> pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(
        wstring(pStr->begin(), pStr->end()));
    if (nullptr == pTex)
        return;

    SetAtlasTex(pTex);
}
