#include "pch.h"
#include "Client/UI/Public/Asset/MaterialUI.h"
#include "Client/UI/Public/Editor/ListUI.h"
#include "Client/UI/Public/Editor/ParamUI.h"
#include "Client/UI/Public/Editor/TreeUI.h"
#include "Client/System/Public/CImGuiMgr.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Rendering/Material/CMaterial.h"

class ListUI;
class TreeNode;
class CMaterial;

MaterialUI::MaterialUI()
	: AssetUI("Material", MATERIAL)
{
}

MaterialUI::~MaterialUI()
{
}

void MaterialUI::Render_Update()
{
	AssetTitle();

	Ptr<CMaterial> pMtrl = static_cast<CMaterial*>(GetAsset().Get());
	Ptr<CGraphicShader> pShader = pMtrl->GetShader();

	string ShaderName;

	if (nullptr == pShader)
		ShaderName = "None";
	else
	{
		ShaderName = WStringToString(pShader->GetKey());
	}

	ImGui::Text("Shader");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150);
	ImGui::InputText("##MeshName", (char*)ShaderName.c_str(), ShaderName.length(),
	                 ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		if (ImGui::AcceptDragDropPayload("ContentUI"))
		{
			const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
			TreeNode* pNode = *static_cast<TreeNode**>(pPayload->Data);
			Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
			if (pAsset->GetAssetType() == GRAPHIC_SHADER)
			{
				pMtrl->SetShader(static_cast<CGraphicShader*>(pAsset.Get()));
				ImGui::SetWindowFocus(nullptr);
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("##ShaderBtn", ImVec2(18.f, 18.f)))
	{
		// ListUI 를 활성화 시키기
		auto pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->SetName("GraphicShader");
		pListUI->SetActive(true);

		// ListUI 에 넣어줄 문자열 정보 가져오기
		pListUI->AddItem("None");

		vector<wstring> vecAssetNames;
		CAssetMgr::GetInst()->GetAssetNames(GRAPHIC_SHADER, vecAssetNames);
		pListUI->AddItem(vecAssetNames);

		// 더블 클릭 시 호출시킬 함수 등록
		pListUI->AddDynamicDoubleCliked(
			this, static_cast<EUI_DELEGATE_2>(&MaterialUI::SelectGraphicShader));
	}


	// Shader 에서 요청하는 파라미터 정보를 출력해준다.
	ShaderParameter();


	// 재질을 파일로 저장하기
	if (ImGui::Button("SAVE"))
	{
		Ptr<CMaterial> pMtrl = static_cast<CMaterial*>(GetAsset().Get());
		assert(pMtrl.Get());

		wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + pMtrl->GetKey();
		pMtrl->Save(strFilePath);
	}
}

void MaterialUI::ShaderParameter()
{
	Ptr<CMaterial> pMtrl = static_cast<CMaterial*>(GetAsset().Get());
	Ptr<CGraphicShader> pShader = pMtrl->GetShader();

	if (nullptr == pShader)
		return;

	const vector<tScalarParam>& vecScalar = pShader->GetScalarParam();

	for (size_t i = 0; i < vecScalar.size(); ++i)
	{
		switch (vecScalar[i].eParam)
		{
		case INT_0:
		case INT_1:
		case INT_2:
		case INT_3:
			{
				auto pData = static_cast<int*>(pMtrl->GetScalarParam(vecScalar[i].eParam));
				int Data = *pData;

				if (ParamUI::Param_Int(vecScalar[i].Desc, &Data, vecScalar[i].Drag))
				{
					pMtrl->SetScalarParam(vecScalar[i].eParam, Data);
				}
			}
			break;
		case FLOAT_0:
		case FLOAT_1:
		case FLOAT_2:
		case FLOAT_3:
			{
				auto pData = static_cast<float*>(pMtrl->GetScalarParam(vecScalar[i].eParam));
				float Data = *pData;

				if (ParamUI::Param_Float(vecScalar[i].Desc, &Data, vecScalar[i].Drag))
				{
					pMtrl->SetScalarParam(vecScalar[i].eParam, Data);
				}
			}
			break;
		case VEC2_0:
		case VEC2_1:
		case VEC2_2:
		case VEC2_3:
			{
				auto pData = static_cast<Vec2*>(pMtrl->GetScalarParam(vecScalar[i].eParam));
				Vec2 Data = *pData;

				if (ParamUI::Param_Vec2(vecScalar[i].Desc, &Data, vecScalar[i].Drag))
				{
					pMtrl->SetScalarParam(vecScalar[i].eParam, Data);
				}
			}
			break;
		case VEC4_0:
		case VEC4_1:
		case VEC4_2:
		case VEC4_3:
			{
				auto pData = static_cast<Vec4*>(pMtrl->GetScalarParam(vecScalar[i].eParam));
				Vec4 Data = *pData;

				if (ParamUI::Param_Vec4(vecScalar[i].Desc, &Data, vecScalar[i].Drag))
				{
					pMtrl->SetScalarParam(vecScalar[i].eParam, Data);
				}
			}
			break;
		case MAT_0:
		case MAT_1:

			break;
		}
	}

	const vector<tTexParam>& vecTex = pShader->GetTexParam();
	for (size_t i = 0; i < vecTex.size(); ++i)
	{
		Ptr<CTexture> pTex = pMtrl->GetTexParam(vecTex[i].eParam);

		if (ParamUI::Param_Tex(vecTex[i].Desc, pTex
		                       , this, static_cast<EUI_DELEGATE_2>(&MaterialUI::SelectTexture)))
		{
			m_OpenTexType = vecTex[i].eParam;
			pMtrl->SetTexParam(vecTex[i].eParam, pTex);
		}
	}
}

void MaterialUI::SelectGraphicShader(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	// 리스트에서 더블킬릭한 항목의 이름을 받아온다.
	auto pListUI = (ListUI*)_ListUI;
	auto pStr = (string*)_SelectString;

	Ptr<CMaterial> pMtrl = static_cast<CMaterial*>(GetAsset().Get());
	if (nullptr == pMtrl)
		return;

	if (*pStr == "None")
	{
		pMtrl->SetShader(nullptr);
		return;
	}

	// 해당 항목 에셋을 찾아서, MeshRenderComponent 가 해당 메시를 참조하게 한다.
	Ptr<CGraphicShader> pShader = CAssetMgr::GetInst()->FindAsset<CGraphicShader>(
		wstring(pStr->begin(), pStr->end()));
	if (nullptr == pShader)
		return;

	pMtrl->SetShader(pShader);
}

void MaterialUI::SelectTexture(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	// 리스트에서 더블킬릭한 항목의 이름을 받아온다.
	auto pListUI = (ListUI*)_ListUI;
	auto pStr = (string*)_SelectString;
	Ptr<CMaterial> pMtrl = static_cast<CMaterial*>(GetAsset().Get());

	if (*pStr == "None")
	{
		pMtrl->SetTexParam(m_OpenTexType, nullptr);
		return;
	}

	// 해당 항목 에셋을 찾아서, MeshRenderComponent 가 해당 메시를 참조하게 한다.
	Ptr<CTexture> pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(
		wstring(pStr->begin(), pStr->end()));
	if (nullptr == pTex)
		return;

	pMtrl->SetTexParam(m_OpenTexType, pTex);
}
