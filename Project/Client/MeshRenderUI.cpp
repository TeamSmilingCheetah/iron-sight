#include "pch.h"
#include "MeshRenderUI.h"

#include <Engine/CAssetMgr.h>
#include <Engine/CMeshRender.h>

#include "CImGuiMgr.h"
#include "ListUI.h"

#include "TreeUI.h"

MeshRenderUI::MeshRenderUI()
    : ComponentUI("MeshRenderUI", COMPONENT_TYPE::MESHRENDER)
{
}

MeshRenderUI::~MeshRenderUI()
{
}

void MeshRenderUI::Render_Update()
{
    ComponentTitle("MeshRender");

    CMeshRender* pMeshRender = GetTargetObject()->MeshRender();

    Ptr<CMesh> pMesh = pMeshRender->GetMesh();
    Ptr<CMaterial> pMtrl = pMeshRender->GetMaterial(0);

    string MeshName;
    if (nullptr == pMesh)
        MeshName = "None";
    else
    {
        MeshName = string(pMesh->GetKey().begin(), pMesh->GetKey().end());
    }

    ImGui::Text("Mesh");
    ImGui::SameLine(100);
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("##MeshName", (char*)MeshName.c_str(), MeshName.length(),
                     ImGuiInputTextFlags_ReadOnly);

    if (ImGui::BeginDragDropTarget())
    {
        if (ImGui::AcceptDragDropPayload("ContentUI"))
        {
            const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
            TreeNode* pNode = *static_cast<TreeNode**>(pPayload->Data);
            Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
            if (pAsset->GetAssetType() == MESH)
            {
                pMeshRender->SetMesh(static_cast<CMesh*>(pAsset.Get()));
                ImGui::SetWindowFocus(nullptr);
            }
        }

        ImGui::EndDragDropTarget();
    }


    ImGui::SameLine();
    if (ImGui::Button("##MeshBtn", ImVec2(18.f, 18.f)))
    {
        // ListUI �� Ȱ��ȭ ��Ű��
        auto pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
        pListUI->SetName("Mesh");
        pListUI->SetActive(true);

        // ListUI �� �־��� ���ڿ� ���� ��������
        pListUI->AddItem("None");

        vector<wstring> vecAssetNames;
        CAssetMgr::GetInst()->GetAssetNames(MESH, vecAssetNames);
        pListUI->AddItem(vecAssetNames);

        // ���� Ŭ�� �� ȣ���ų �Լ� ���
        pListUI->AddDynamicDoubleCliked(
            this, static_cast<EUI_DELEGATE_2>(&MeshRenderUI::SelectMesh));
    }


    string MtrlName;
    if (nullptr == pMtrl)
        MtrlName = "None";
    else
    {
        MtrlName = string(pMtrl->GetKey().begin(), pMtrl->GetKey().end());
    }

    ImGui::Text("Material");
    ImGui::SameLine(100);
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("##MtrlName", (char*)MtrlName.c_str(), MtrlName.length(),
                     ImGuiInputTextFlags_ReadOnly);

    if (ImGui::BeginDragDropTarget())
    {
        if (ImGui::AcceptDragDropPayload("ContentUI"))
        {
            const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
            TreeNode* pNode = *static_cast<TreeNode**>(pPayload->Data);
            Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
            if (pAsset->GetAssetType() == MATERIAL)
            {
                pMeshRender->SetMaterial(static_cast<CMaterial*>(pAsset.Get()), 0);
                ImGui::SetWindowFocus(nullptr);
            }
        }

        ImGui::EndDragDropTarget();
    }


    ImGui::SameLine();
    if (ImGui::Button("##MtrlBtn", ImVec2(18.f, 18.f)))
    {
        // ListUI �� Ȱ��ȭ ��Ű��
        auto pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
        pListUI->SetName("Material");
        pListUI->SetActive(true);

        // ListUI �� �־��� ���ڿ� ���� ��������
        pListUI->AddItem("None");

        vector<wstring> vecAssetNames;
        CAssetMgr::GetInst()->GetAssetNames(MATERIAL, vecAssetNames);
        pListUI->AddItem(vecAssetNames);

        // ���� Ŭ�� �� ȣ���ų �Լ� ���
        pListUI->AddDynamicDoubleCliked(
            this, static_cast<EUI_DELEGATE_2>(&MeshRenderUI::SelectMaterial));
    }
}

void MeshRenderUI::SelectMesh(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
    // ����Ʈ���� ����ų���� �׸��� �̸��� �޾ƿ´�.
    auto pListUI = (ListUI*)_ListUI;
    auto pStr = (string*)_SelectString;

    if (*pStr == "None")
    {
        GetTargetObject()->MeshRender()->SetMesh(nullptr);
        return;
    }

    // �ش� �׸� ������ ã�Ƽ�, MeshRenderComponent �� �ش� �޽ø� �����ϰ� �Ѵ�.
    Ptr<CMesh> pMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(wstring(pStr->begin(), pStr->end()));
    if (nullptr == pMesh)
        return;

    GetTargetObject()->MeshRender()->SetMesh(pMesh);
}

void MeshRenderUI::SelectMaterial(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
    // ����Ʈ���� ����ų���� �׸��� �̸��� �޾ƿ´�.
    auto pListUI = (ListUI*)_ListUI;
    auto pStr = (string*)_SelectString;

    if (*pStr == "None")
    {
        GetTargetObject()->MeshRender()->SetMaterial(nullptr, 0);
        return;
    }

    // �ش� �׸� ������ ã�Ƽ�, MeshRenderComponent �� �ش� �޽ø� �����ϰ� �Ѵ�.
    Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(
        wstring(pStr->begin(), pStr->end()));
    if (nullptr == pMtrl)
        return;

    GetTargetObject()->MeshRender()->SetMaterial(pMtrl, 0);
}
