#include "pch.h"

#include "Client/UI/Public/Editor/AddObjectUI.h"
#include "Client/UI/Public/Editor/ListUI.h"
#include "Client/System/Public/CImGuiMgr.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"

AddObjectUI::AddObjectUI()
	: EditorUI("AddObjectUI")
	, m_Object(nullptr)
	, m_Idx(0)
	, m_MoveWithChild(false)
{
}

AddObjectUI::~AddObjectUI()
{
	DELETE(m_Object);
}


void AddObjectUI::Render_Update()
{
	string ObjName;

	if (nullptr != m_Object)
		ObjName = WStringToString(m_Object->GetName());

	if (ImGui::Button("Load Prefab"))
	{
		wstring SelectedPrefabPath = L"";
		OPENFILENAME ofn;
		wchar_t filePath[MAX_PATH] = L"";

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = sizeof(filePath);
		ofn.lpstrFilter = L"Prefab\0*.pref\0ALL\0*.*";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
		strFilePath += L"Prefab";
		ofn.lpstrInitialDir = strFilePath.c_str();

		if (GetOpenFileName(&ofn))
		{
			SelectedPrefabPath = filePath; // 선택한 경로 저장
		}
		if (L"" == SelectedPrefabPath)
			return;

		wstring RelativePath = SelectedPrefabPath.substr(SelectedPrefabPath.find(L"Prefab"));

		// 해당 항목 에셋을 찾아서, MeshRenderComponent 가 해당 메시를 참조하게 한다.
		Ptr<CPrefab> pPrefab = CAssetMgr::GetInst()->FindAsset<CPrefab>(RelativePath);

		pPrefab = CAssetMgr::GetInst()->Load<CPrefab>(RelativePath, RelativePath);
		m_Object = pPrefab->Instantiate();
	}

	//if (ImGui::Button("Load Prefab"))
	//{
	//	// ListUI 를 활성화 시키기
	//	ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
	//	pListUI->SetName("Material");
	//	pListUI->SetActive(true);

	//	// ListUI 에 넣어줄 문자열 정보 가져오기
	//	pListUI->AddItem("None");

	//	vector<wstring> vecAssetNames;
	//	CAssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::PREFAB, vecAssetNames);
	//	pListUI->AddItem(vecAssetNames);

	//	// 더블 클릭 시 호출시킬 함수 등록
	//	pListUI->AddDynamicDoubleCliked(this, (EUI_DELEGATE_2)&AddObjectUI::SelectPrefab);
	//}

	ImGui::Text("Current Object");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150);
	ImGui::InputText("##ObjectName", (char*)ObjName.c_str(), ObjName.length(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Layer Idx");
	ImGui::SameLine(100);
	ImGui::InputInt("##LayerIdx", &m_Idx);

	// 현재 FlipbookName
	ImGui::Text("Layer List");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::Text("0 : Background");
	ImGui::Text("1 : Structure");
	ImGui::Text("2 : Default");
	ImGui::Text("3 : PlayerTPS");
	ImGui::Text("4 : PlayerFPS");
	ImGui::Text("5 : PlayerObject");
	ImGui::Text("6 : Item");
	ImGui::Text("7 : MonsterObject");
	ImGui::Text("8 : UI");
	ImGui::Text("9 : ObjectPool");
	ImGui::Text("10 : BulletLayer");
	ImGui::PopStyleColor();


	bool Move = m_MoveWithChild;

	ImGui::Text("MoveWithChild");
	ImGui::SameLine(100);
	ImGui::Checkbox("##MoveWithChild", &Move);
	m_MoveWithChild = Move;
	Move = m_MoveWithChild;

	if (ImGui::Button("Add"))
	{
		CreateObject(m_Object, m_Idx, m_MoveWithChild);
		m_Object = nullptr;
	}
}

//void AddObjectUI::SelectPrefab(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
//{
//	// 리스트에서 더블킬릭한 항목의 이름을 받아온다.
//	ListUI* pListUI = (ListUI*)_ListUI;
//	string* pStr = (string*)_SelectString;
//
//	if (*pStr == "None")
//	{
//		return;
//	}
//
//	// 해당 항목 에셋을 찾아서, MeshRenderComponent 가 해당 메시를 참조하게 한다.
//	Ptr<CPrefab> pPrefab = CAssetMgr::GetInst()->FindAsset<CPrefab>(wstring(pStr->begin(), pStr->end()));
//	if (nullptr == pPrefab)
//		return;
//
//	m_Object = pPrefab->Instantiate();
//}
