#include "pch.h"
#include "Client/UI/Public/Component/MeshRenderUI.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Client/System/Public/CImGuiMgr.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Client/UI/Public/Editor/ListUI.h"
#include "Client/UI/Public/Editor/TreeUI.h"

class ListUI;
class TreeNode;
class CMeshRender;

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
		MeshName = WStringToString(pMesh->GetKey());
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
		// ListUI 를 활성화 시키기
		auto pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->SetName("Mesh");
		pListUI->SetActive(true);

		// ListUI 에 넣어줄 문자열 정보 가져오기
		pListUI->AddItem("None");

		vector<wstring> vecAssetNames;
		CAssetMgr::GetInst()->GetAssetNames(MESH, vecAssetNames);
		pListUI->AddItem(vecAssetNames);

		// 더블 클릭 시 호출시킬 함수 등록
		pListUI->AddDynamicDoubleCliked(
			this, static_cast<EUI_DELEGATE_2>(&MeshRenderUI::SelectMesh));
	}


	string MtrlName;
	if (nullptr == pMtrl)
		MtrlName = "None";
	else
	{
		MtrlName = WStringToString(pMtrl->GetKey());
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
		// ListUI 를 활성화 시키기
		auto pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->SetName("Material");
		pListUI->SetActive(true);

		// ListUI 에 넣어줄 문자열 정보 가져오기
		pListUI->AddItem("None");

		vector<wstring> vecAssetNames;
		CAssetMgr::GetInst()->GetAssetNames(MATERIAL, vecAssetNames);
		pListUI->AddItem(vecAssetNames);

		// 더블 클릭 시 호출시킬 함수 등록
		pListUI->AddDynamicDoubleCliked(
			this, static_cast<EUI_DELEGATE_2>(&MeshRenderUI::SelectMaterial));
	}

	int MaterialCount = pMeshRender->GetMaterialCount();
	if (1 <= MaterialCount)
	{
		for (int i = 0; i < MaterialCount; ++i)
		{
			string mtrlID = "Mtrl " + std::to_string(i);
			pMtrl = pMeshRender->GetMaterial(i);
			MtrlName = WStringToString(pMtrl->GetKey());
			ImGui::SetNextItemWidth(400);
			ImGui::InputText(mtrlID.c_str(), (char*)MtrlName.c_str(), MtrlName.length(),
				ImGuiInputTextFlags_ReadOnly);
		}		
	}


	if (ImGui::Button("DELETE##MeshRender"))
	{
		DeleteComponent(COMPONENT_TYPE::MESHRENDER);
	}
}

void MeshRenderUI::SelectMesh(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	// 리스트에서 더블킬릭한 항목의 이름을 받아온다.
	auto pListUI = (ListUI*)_ListUI;
	auto pStr = (string*)_SelectString;

	if (*pStr == "None")
	{
		GetTargetObject()->MeshRender()->SetMesh(nullptr);
		return;
	}

	// 해당 항목 에셋을 찾아서, MeshRenderComponent 가 해당 메시를 참조하게 한다.
	Ptr<CMesh> pMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(wstring(pStr->begin(), pStr->end()));
	if (nullptr == pMesh)
		return;

	GetTargetObject()->MeshRender()->SetMesh(pMesh);
}

void MeshRenderUI::SelectMaterial(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	// 리스트에서 더블킬릭한 항목의 이름을 받아온다.
	auto pListUI = (ListUI*)_ListUI;
	auto pStr = (string*)_SelectString;

	if (*pStr == "None")
	{
		GetTargetObject()->MeshRender()->SetMaterial(nullptr, 0);
		return;
	}

	// 해당 항목 에셋을 찾아서, MeshRenderComponent 가 해당 메시를 참조하게 한다.
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(
		wstring(pStr->begin(), pStr->end()));
	if (nullptr == pMtrl)
		return;

	GetTargetObject()->MeshRender()->SetMaterial(pMtrl, 0);
}
