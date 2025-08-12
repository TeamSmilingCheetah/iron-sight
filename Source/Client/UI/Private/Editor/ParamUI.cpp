#include "pch.h"
#include "Client/UI/Public/Editor/ParamUI.h"
#include "Client/imgui/imgui.h"
#include "Client/System/Public/CImGuiMgr.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Engine/System/Public/Manager/AssetManager.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Client/UI/Public/Editor/ListUI.h"
#include "Client/UI/Public/Editor/TreeUI.h"
#include "Client/UI/Public/Editor/ContentUI.h"

class ListUI;
class TreeNode;
UINT ParamUI::g_ParamID = 0;

bool ParamUI::Param_Int(const string& _Desc, int* _Data, bool _Drag)
{
	ImGui::Text(_Desc.c_str());
	ImGui::SameLine(120);

	char szID[255] = {};
	sprintf_s(szID, "##Param_Int_%d", g_ParamID++);

	if (_Drag)
	{
		if (ImGui::DragInt(szID, _Data, 1.f))
		{
			return true;
		}
	}
	else
	{
		if (ImGui::InputInt(szID, _Data, 0))
		{
			return true;
		}
	}

	return false;
}

bool ParamUI::Param_Float(const string& _Desc, float* _Data, bool _Drag)
{
	ImGui::Text(_Desc.c_str());
	ImGui::SameLine(120);

	char szID[255] = {};
	sprintf_s(szID, "##Param_Float_%d", g_ParamID++);


	if (_Drag)
	{
		if (ImGui::DragFloat(szID, _Data, 0.05f))
		{
			return true;
		}
	}
	else
	{
		if (ImGui::InputFloat(szID, _Data, 0))
		{
			return true;
		}
	}

	return false;
}

bool ParamUI::Param_Vec2(const string& _Desc, Vec2* _Data, bool _Drag)
{
	ImGui::Text(_Desc.c_str());
	ImGui::SameLine(120);

	char szID[255] = {};
	sprintf_s(szID, "##Param_Vec2_%d", g_ParamID++);


	float arr[2] = {_Data->x, _Data->y};

	if (_Drag)
	{
		if (ImGui::DragFloat2(szID, arr, 1.f))
		{
			_Data->x = arr[0];
			_Data->y = arr[1];
			return true;
		}
	}
	else
	{
		if (ImGui::InputFloat2(szID, arr, nullptr))
		{
			_Data->x = arr[0];
			_Data->y = arr[1];
			return true;
		}
	}

	return false;
}

bool ParamUI::Param_Vec4(const string& _Desc, Vec4* _Data, bool _Drag)
{
	ImGui::Text(_Desc.c_str());
	ImGui::SameLine(120);

	char szID[255] = {};
	sprintf_s(szID, "##Param_Vec4_%d", g_ParamID++);


	float arr[4] = {_Data->x, _Data->y, _Data->z, _Data->w};

	if (_Drag)
	{
		if (ImGui::DragFloat4(szID, arr, 1.f))
		{
			*_Data = Vec4(arr[0], arr[1], arr[2], arr[3]);
			return true;
		}
	}
	else
	{
		if (ImGui::InputFloat(szID, arr, 0))
		{
			*_Data = Vec4(arr[0], arr[1], arr[2], arr[3]);
			return true;
		}
	}

	return false;
}

bool ParamUI::Param_Tex(const string& _Desc, Ptr<CTexture>& _Tex
						, EditorUI* _Inst, EUI_DELEGATE_2 _MemFunc)
{
	char szID[255] = {};

	ImGui::Text(_Desc.c_str());

	ImVec2 uv_min = ImVec2(0.0f, 0.0f);
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

	if (nullptr == _Tex)
		ImGui::Image(nullptr, ImVec2(100.f, 100.f), uv_min, uv_max, tint_col, border_col);
	else
		ImGui::Image(_Tex->GetSRV().Get(), ImVec2(100.f, 100.f), uv_min, uv_max, tint_col,
					 border_col);

	if (ImGui::BeginDragDropTarget())
	{
		if (ImGui::AcceptDragDropPayload("ContentUI"))
		{
			const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
			TreeNode* pNode = *static_cast<TreeNode**>(pPayload->Data);
			tFSNode* pFSNode = reinterpret_cast<tFSNode*>(pNode->GetData());
			Ptr<FAsset> pAsset = pFSNode->Asset;
			if (pAsset == nullptr)
				pAsset = ContentUI::LoadAsset(pFSNode);

			if (pAsset->GetAssetType() == TEXTURE)
			{
				_Tex = static_cast<CTexture*>(pAsset.Get());
				ImGui::SetWindowFocus(nullptr);
				return true;
			}
		}

		ImGui::EndDragDropTarget();
	}


	if (_Inst && _MemFunc)
	{
		ImGui::SameLine();

		sprintf_s(szID, "##TexBtn_%d", g_ParamID++);
		if (ImGui::Button(szID, ImVec2(18.f, 18.f)))
		{
			// ListUI 를 활성화 시키기
			ListUI* pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
			pListUI->SetName("Texture");
			pListUI->SetActive(true);

			// ListUI 에 넣어줄 문자열 정보 가져오기
			pListUI->AddItem("None");

			vector<wstring> vecAssetNames;
			FAssetManager::GetInst()->GetAssetNames(TEXTURE, vecAssetNames);
			pListUI->AddItem(vecAssetNames);

			// 더블 클릭 시 호출시킬 함수 등록
			pListUI->AddDynamicDoubleCliked(_Inst, _MemFunc);

			return true;
		}
	}

	return false;
}

bool ParamUI::Param_Prefab(const string& _Desc, Ptr<CPrefab>& _Prefab, EditorUI* _Inst,
						   EUI_DELEGATE_2 _MemFunc)
{
	char szID[255] = {};

	ImGui::Text(_Desc.c_str());
	ImGui::SameLine(100);

	string strKey;
	if (nullptr != _Prefab)
		strKey = WStringToString(_Prefab->GetKey());
	else
		strKey = "NONE";

	sprintf_s(szID, "##PrefabName_%d", g_ParamID++);
	ImGui::InputText(szID, (char*)strKey.c_str(), strKey.length(), ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		if (ImGui::AcceptDragDropPayload("ContentUI"))
		{
			const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
			TreeNode* pNode = *reinterpret_cast<TreeNode**>(pPayload->Data);
			tFSNode* pFSNode = reinterpret_cast<tFSNode*>(pNode->GetData());
			Ptr<FAsset> pAsset = pFSNode->Asset;
			if (pAsset == nullptr)
				pAsset = ContentUI::LoadAsset(pFSNode);

			if (pAsset->GetAssetType() == PREFAB)
			{
				_Prefab = static_cast<CPrefab*>(pAsset.Get());
				ImGui::SetWindowFocus(nullptr);
				return true;
			}
		}

		ImGui::EndDragDropTarget();
	}

	if (_Inst && _MemFunc)
	{
		ImGui::SameLine();

		sprintf_s(szID, "##PrefabBtn_%d", g_ParamID++);
		if (ImGui::Button(szID, ImVec2(18.f, 18.f)))
		{
			// ListUI 를 활성화 시키기
			ListUI* pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
			pListUI->SetName("Prefab");
			pListUI->SetActive(true);

			// ListUI 에 넣어줄 문자열 정보 가져오기
			pListUI->AddItem("None");

			vector<wstring> vecAssetNames;
			FAssetManager::GetInst()->GetAssetNames(PREFAB, vecAssetNames);
			pListUI->AddItem(vecAssetNames);

			// 더블 클릭 시 호출시킬 함수 등록
			pListUI->AddDynamicDoubleCliked(_Inst, _MemFunc);

			return true;
		}
	}

	return false;
}

bool ParamUI::Param_GameObject(const string& _Desc, CGameObject** _Object)
{
	char szID[255] = {};

	ImGui::Text(_Desc.c_str());
	ImGui::SameLine(100);

	string strKey;
	if (nullptr != *_Object)
		strKey = WStringToString((*_Object)->GetName());
	else
		strKey = "NONE";

	sprintf_s(szID, "##PrefabName_%d", g_ParamID++);
	ImGui::InputText(szID, (char*)strKey.c_str(), strKey.length(), ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		if (ImGui::AcceptDragDropPayload("Outliner"))
		{
			const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
			TreeNode* pNode = *static_cast<TreeNode**>(pPayload->Data);
			*_Object = reinterpret_cast<CGameObject*>(pNode->GetData());

			ImGui::SetWindowFocus(nullptr);
			return true;
		}

		ImGui::EndDragDropTarget();
	}

	return false;
}
