#include "pch.h"
#include "Client/UI/Public/Editor/Outliner.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Actor/CLayer.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CTaskMgr.h"
#include "Engine/System/Public/Manager/CRenderMgr.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Client/System/Public/CImGuiMgr.h"
#include "Client/UI/Public/Editor/Inspector.h"
#include "Client/UI/Public/Editor/TreeUI.h"

class Inspector;
class CLayer;

Outliner::Outliner()
	: EditorUI("Outliner")
{
	m_Tree = static_cast<TreeUI*>(AddChildUI(new TreeUI));
	m_Tree->SetName("Outliner");
	m_Tree->ShowRoot(false);
	m_Tree->SelfDragDrop(true);
	m_Tree->RightOption(true, this);

	m_Tree->AddDynamicSelect(this, static_cast<EUI_DELEGATE_1>(&Outliner::SelectGameObject));
	m_Tree->AddDynamicSelfDragDrop(this, static_cast<EUI_DELEGATE_2>(&Outliner::DragDrop));
	m_Tree->AddRightItemDelegate((EUI_DELEGATE_1)&Outliner::ChangeName_Outliner);
	m_Tree->AddRightItemDelegate((EUI_DELEGATE_1)&Outliner::CopyObject);
	m_Tree->AddRightItemDelegate((EUI_DELEGATE_1)&Outliner::DeleteObject);
	m_Tree->AddRightItemDelegate((EUI_DELEGATE_1)&Outliner::MoveToObject);
	m_Tree->AddRightItemDelegate((EUI_DELEGATE_1)&Outliner::MakePrefab);
	m_Tree->AddRightSpaceDelegate((EUI_DELEGATE_1)&Outliner::CreateObject_Outliner);
}

Outliner::~Outliner()
{
}

void Outliner::Render_Update()
{
	if (CTaskMgr::GetInst()->IsLevelChanged())
	{
		RenewGameObject();
	}
}

void Outliner::RenewGameObject()
{
	// 새로 만들기 전에 초기화
	m_Tree->Clear();

	// 최상위 부모 노드생성
	TreeNode* pRootNode = m_Tree->AddItem(nullptr, "RootNode", 0);

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	for (size_t i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurLevel->GetLayer(static_cast<int>(i));
		string LayerName = WStringToString(pLayer->GetName());
		TreeNode* pListNode = m_Tree->AddItem(pRootNode, LayerName, (DWORD_PTR)i, false, true);

		const vector<CGameObject*>& vecParents = pLayer->GetParentObjects();
		for (size_t j = 0; j < vecParents.size(); ++j)
		{
			AddGameObject(pListNode, vecParents[j]);
		}
	}
}

void Outliner::AddGameObject(TreeNode* _ParentNode, CGameObject* _Object)
{
	auto Name = WStringToString(_Object->GetName());

	TreeNode* pCurNode = m_Tree->AddItem(_ParentNode, Name, (DWORD_PTR)_Object);

	const vector<CGameObject*>& vecChild = _Object->GetChild();

	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		AddGameObject(pCurNode, vecChild[i]);
	}
}

void Outliner::SelectGameObject(DWORD_PTR _TreeNode)
{
	TreeNode* pNode = (TreeNode*)_TreeNode;

	// 100보다 작다면 이건 오브젝트 포인터가 아니다.
	if (static_cast<DWORD_PTR>(100LL) > pNode->GetData())
		return;

	CGameObject* pTarget = reinterpret_cast<CGameObject*>(pNode->GetData());

	Inspector* pInspector = (Inspector*)CImGuiMgr::GetInst()->FindUI("Inspector");
	pInspector->SetTargetObject(pTarget);
}

void Outliner::DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode)
{
	TreeNode* pDragged = (TreeNode*)_DragNode;
	TreeNode* pDropped = (TreeNode*)_DropNode;

	// 드래그된 노드의 데이터 가져오기
	DWORD_PTR DragData = pDragged->GetData();

	// 드래그된 항목이 레이어 노드인지 먼저 확인 (값이 1000보다 작으면 레이어 노드)
	if (DragData <= static_cast<DWORD_PTR>(1000LL))
	{
		// 레이어 노드는 드래그 앤 드롭 대상이 아님
		return;
	}

	CGameObject* pDragObj = reinterpret_cast<CGameObject*>(pDragged->GetData());
	CGameObject* pDropObj = nullptr;

	if (pDragged && !pDragObj)
	{
		// 드래그된 노드가 CGameObject가 아닌 경우 return
		return;
	}

	if (pDropped)
	{
		// pDropped의 데이터를 DWORD_PTR에서 CGameObject*로 변환
		DWORD_PTR DropData = pDropped->GetData();

		// DropData가 CGameObject*인지 long long인지 구별하기 위한 확인
		// 1000 보다 크면 포인터로 본다.
		if (DropData > static_cast<DWORD_PTR>(1000LL))
		{
			pDropObj = reinterpret_cast<CGameObject*>(DropData);

			if (pDropObj->IsAncestor(pDragObj))
				return;
		}
		// DropData가 long long이라면
		else
		{
			LONGLONG LayerIdx = static_cast<LONGLONG>(DropData);

			ChangeLayer(pDragObj, LayerIdx);
		}
	}

	AddChild(pDropObj, pDragObj);
}


void Outliner::CreateObject_Outliner(Ptr<CMesh> _pMesh)
{
	int idx = 0;
	for (; idx < 32; ++idx)
	{
		if (!(m_ObjectIdx & 1 << idx)) break;
	}

	m_ObjectIdx |= 1 << idx;

	wchar_t szBuff[15]{};
	swprintf_s(szBuff, L"New Object %d", idx);

	CGameObject* pObj = new CGameObject;
	pObj->SetName(wstring(szBuff));

	if (_pMesh != nullptr)
	{
		pObj->AddComponent(new CMeshRender);

		Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std3D_DeferredMtrl");
		pObj->MeshRender()->SetMesh(_pMesh);
		pObj->MeshRender()->SetMaterial(pMtrl, 0);
	}

	pObj->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

	CreateObject(pObj, 2, false);
}

void Outliner::CheckDefaultName(const string& _OldName)
{
	string prefix = "New Object ";

	if (_OldName.size() > prefix.size() && _OldName.substr(0, prefix.size()) == prefix)
	{
		string numPart = _OldName.substr(prefix.size());  // 숫자 부분 추출

		bool isNum = true;
		for (char ch : numPart) {
			if (!isdigit(ch))
			{
				isNum = false;
				break;
			}
		}

		if (isNum)
			ClearObjectIdx(stoi(numPart));
	}
}

void Outliner::ChangeName_Outliner(DWORD_PTR _TreeNode)
{
	// 이름 설정
	if (ImGui::Selectable("Change Name", false, ImGuiSelectableFlags_DontClosePopups))
	{
		ImGui::OpenPopup("Name_Setting_popup");

		TreeNode* pNode = (TreeNode*)_TreeNode;
		m_TargetObject = (CGameObject*)pNode->GetData();
	}

	if (ImGui::BeginPopup("Name_Setting_popup"))
	{
		char szBuff[50]{};
		string strName = WStringToString(m_TargetObject->GetName());
		strcpy_s(szBuff, strName.c_str());
		if (ImGui::InputText("##ObjectNameSet", szBuff, sizeof(szBuff), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			// 새로 오브젝트 만들엇을 때 주는 디폴트 이름인지 체크
			CheckDefaultName(strName);

			strName = szBuff;
			static wstring wstrName(L"");
			wstrName = wstring(strName.begin(), strName.end());

			ChangeName(static_cast<CEntity*>(m_TargetObject), &wstrName);

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void Outliner::MakePrefab(DWORD_PTR _TreeNode)
{
	if (ImGui::MenuItem("Make Prefab"))
	{
		TreeNode* pNode = (TreeNode*)_TreeNode;
		m_TargetObject = (CGameObject*)pNode->GetData();

		Ptr<CPrefab> pPrefab = new CPrefab;
		pPrefab->SetProtoObject(m_TargetObject->Clone());
		CAssetMgr::GetInst()->AddAsset(m_TargetObject->GetName(), pPrefab);
	}
}

void Outliner::CopyObject(DWORD_PTR _TreeNode)
{
	if (ImGui::MenuItem("Copy Object"))
	{
		TreeNode* pNode = (TreeNode*)_TreeNode;
		m_TargetObject = (CGameObject*)pNode->GetData();

		CGameObject* pNewObject = m_TargetObject->Clone();
		CreateObject(pNewObject, m_TargetObject->GetLayerIdx(), false);
	}
}

void Outliner::DeleteObject(DWORD_PTR _TreeNode)
{
	if (ImGui::MenuItem("Delete Object"))
	{
		TreeNode* pNode = (TreeNode*)_TreeNode;
		m_TargetObject = (CGameObject*)pNode->GetData();

		// Default Object인 경우 ID 비워주기
		CheckDefaultName(WStringToString(m_TargetObject->GetName()));

		DestroyObject(m_TargetObject);
		Inspector* pInspector = (Inspector*)CImGuiMgr::GetInst()->FindUI("Inspector");
		pInspector->SetTargetObject(nullptr);
	}
}

void Outliner::MoveToObject(DWORD_PTR _TreeNode)
{
	if (ImGui::MenuItem("Move to Object"))
	{
		TreeNode* pNode = (TreeNode*)_TreeNode;
		m_TargetObject = (CGameObject*)pNode->GetData();

		// 오브젝트 위치로 이동 (Editor Mode)
		if (CRenderMgr::GetInst()->IsEditorMode()) // Editor
		{
			CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();
			pCam->Transform()->SetRelativePos(m_TargetObject->Transform()->GetWorldPos());
		}
	}
}

void Outliner::CreateObject_Outliner(DWORD_PTR _Nothing)
{
	// Engine Mesh 또는 Empty Object를 만듦.
	if (ImGui::BeginMenu("Create Object"))
	{
		const map<wstring, Ptr<CAsset>>& m_mapMesh = CAssetMgr::GetInst()->GetAssets(ASSET_TYPE::MESH);

		if (ImGui::MenuItem("Empty"))
		{
			CreateObject_Outliner(nullptr);
		}

		// Engine Mesh에 대해서만 적용
		for (auto iter = m_mapMesh.begin(); iter != m_mapMesh.end(); ++iter)
		{
			if (!iter->second->IsEngineAsset())
				continue;

			string meshName = WStringToString(iter->second->GetKey());

			if (ImGui::MenuItem(meshName.c_str()))
			{
				CreateObject_Outliner(static_cast<CMesh*>(iter->second.Get()));
			}
		}

		ImGui::EndMenu();
	}
}

