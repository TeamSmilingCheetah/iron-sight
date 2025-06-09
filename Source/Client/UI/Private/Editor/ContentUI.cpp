#include "pch.h"
#include "Client/UI/Public/Editor/ContentUI.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CTaskMgr.h"
#include "Client/System/Public/CImGuiMgr.h"
#include "Client/UI/Public/Editor/Inspector.h"
#include "Client/UI/Public/Editor/TreeUI.h"

#include "Engine/Core/Public/CEngine.h"

class Inspector;

ContentUI::ContentUI()
	: EditorUI("Content")
{
	m_Tree = static_cast<TreeUI*>(AddChildUI(new TreeUI));
	m_Tree->SetName("ContentUI");
	m_Tree->ShowRoot(false);
	m_Tree->RightOption(true, this);

	m_Tree->AddDynamicSelect(this, static_cast<EUI_DELEGATE_1>(&ContentUI::SelectAsset));
	m_Tree->AddRightItemDelegate((EUI_DELEGATE_1)&ContentUI::ChangeName_ContentUI);
	m_Tree->AddRightItemDelegate((EUI_DELEGATE_1)&ContentUI::CopyAsset);


	// Content 폴더안에 있는 모든 에셋을 메모리로 로딩
	ReloadContent();

	// 현재 CAssetMgr 에 로딩된 에셋의 정보를 Tree 에 출력
	RenewContent();
}

ContentUI::~ContentUI()
{
}

void ContentUI::Render_Update()
{
	if (CAssetMgr::GetInst()->IsAssetChanged())
	{
		RenewContent();
	}
}

void ContentUI::Reset()
{
	ReloadContent();

	RenewContent();
}

void ContentUI::RenewContent()
{
	m_Tree->Clear();

	TreeNode* pRootNode = m_Tree->AddItem(nullptr, "RootNode", 0);

	for (UINT i = 0; i < static_cast<UINT>(ASSET_TYPE::END); ++i)
	{
		TreeNode* pAssetNode = m_Tree->AddItem(pRootNode, ASSET_TYPE_STRING[i], 0, true);

		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssets(
			static_cast<ASSET_TYPE>(i));

		for (const auto& pair : mapAsset)
		{
			m_Tree->AddItem(pAssetNode, WStringToString(pair.first),
			                (DWORD_PTR)pair.second.Get());
		}
	}
}

void ContentUI::ReloadContent()
{
	// Content 폴더 안에있는 모든 에셋의 경로를 찾아낸다.
	m_vecAssetPath.clear();

	// TEST : 메모맆 프로파일링 asset 타입별로
	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		m_vecAssetPathByType[i].clear();
	}

	FindAssetPath(CPathMgr::GetInst()->GetContentPath());

	//for (size_t i = 0; i < m_vecAssetPath.size(); ++i)
	//{
	//	ASSET_TYPE Type = GetAssetType(m_vecAssetPath[i]);
	//
	//	switch (Type)
	//	{
	//	case MESH:
	//		CAssetMgr::GetInst()->Load<CMesh>(m_vecAssetPath[i], m_vecAssetPath[i]);
	//		break;
	//	case MESH_DATA:
	//		CAssetMgr::GetInst()->Load<CMeshData>(m_vecAssetPath[i], m_vecAssetPath[i]);
	//		break;
	//	case TEXTURE:
	//		CAssetMgr::GetInst()->Load<CTexture>(m_vecAssetPath[i], m_vecAssetPath[i]);
	//		break;
	//	case SOUND:
	//		CAssetMgr::GetInst()->Load<CSound>(m_vecAssetPath[i], m_vecAssetPath[i]);
	//		break;
	//	case PREFAB:
	//		CAssetMgr::GetInst()->Load<CPrefab>(m_vecAssetPath[i], m_vecAssetPath[i]);
	//		break;
	//	case FLIPBOOK:
	//		CAssetMgr::GetInst()->Load<CFlipbook>(m_vecAssetPath[i], m_vecAssetPath[i]);
	//		break;
	//	case ANIMATION:
	//		CAssetMgr::GetInst()->Load<CAnimation>(m_vecAssetPath[i], m_vecAssetPath[i]);
	//		break;
	//	case SPRITE:
	//		CAssetMgr::GetInst()->Load<CSprite>(m_vecAssetPath[i], m_vecAssetPath[i]);
	//		break;
	//	case MATERIAL:
	//		CAssetMgr::GetInst()->Load<CMaterial>(m_vecAssetPath[i], m_vecAssetPath[i]);
	//		break;
	//	}
	//}

	// TEST : 메모리 프로파일링 asset 타입별로
	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		for (int j = 0; j < m_vecAssetPathByType[i].size(); ++j)
		{
			switch ((ASSET_TYPE)i)
			{
			case MESH:
				CAssetMgr::GetInst()->Load<CMesh>(m_vecAssetPathByType[i][j], m_vecAssetPathByType[i][j]);
				break;
			case MESH_DATA:
				CAssetMgr::GetInst()->Load<CMeshData>(m_vecAssetPathByType[i][j], m_vecAssetPathByType[i][j]);
				break;
			case TEXTURE:
				CAssetMgr::GetInst()->Load<CTexture>(m_vecAssetPathByType[i][j], m_vecAssetPathByType[i][j]);
				break;
			case SOUND:
				CAssetMgr::GetInst()->Load<CSound>(m_vecAssetPathByType[i][j], m_vecAssetPathByType[i][j]);
				break;
			case PREFAB:
				CAssetMgr::GetInst()->Load<CPrefab>(m_vecAssetPathByType[i][j], m_vecAssetPathByType[i][j]);
				break;
			case FLIPBOOK:
				CAssetMgr::GetInst()->Load<CFlipbook>(m_vecAssetPathByType[i][j], m_vecAssetPathByType[i][j]);
				break;
			case ANIMATION:
				CAssetMgr::GetInst()->Load<CAnimation>(m_vecAssetPathByType[i][j], m_vecAssetPathByType[i][j]);
				break;
			case SPRITE:
				CAssetMgr::GetInst()->Load<CSprite>(m_vecAssetPathByType[i][j], m_vecAssetPathByType[i][j]);
				break;
			case MATERIAL:
				CAssetMgr::GetInst()->Load<CMaterial>(m_vecAssetPathByType[i][j], m_vecAssetPathByType[i][j]);
				break;
			}
		}

		CEngine::GetInst()->PrintMemoryUsage(ASSET_TYPE_STRING[i]);
	}

	// TEST : 메모리 프로파일링 텍스쳐 ScratchImage 해제
	CAssetMgr::GetInst()->ReleaseTextureSource();
	CEngine::GetInst()->PrintMemoryUsage("Scratch Released");

	// 에셋의 원본 파일이 존재하는지 체크
	for (UINT i = 0; i < END; ++i)
	{
		wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssets(
			static_cast<ASSET_TYPE>(i));

		for (const auto& pair : mapAsset)
		{
			// Engine Asset 인 경우, 원본파일이 존재하지 않으므로, 건너뛴다.
			if (pair.second->IsEngineAsset())
				continue;

			// 메모리에 로딩된 에셋의 원본파일 경로 계산
			wstring strFilePath = ContentPath + pair.second->GetRelativePath();

			// 원본파일이 없으면(삭제되었으면), 메모리상에 해당 에셋도 삭제시킨다.
			if (false == exists(strFilePath.c_str()))
			{
				MessageBox(nullptr, L"에셋의 원본 파일이 삭제됨", L"에셋 파일 삭제 감지", MB_OK);

				tTask task = {TASK_TYPE::DELETE_ASSET, (DWORD_PTR)pair.second.Get()};
				CTaskMgr::GetInst()->AddTask(task);
			}
		}
	}
}

void ContentUI::FindAssetPath(const wstring& _FolderPath)
{
	wstring Path = _FolderPath + L"*.*";

	WIN32_FIND_DATA FindData = {};

	HANDLE hHandle = FindFirstFile(Path.c_str(), &FindData);

	while (FindNextFile(hHandle, &FindData))
	{
		// 찾은 파일이 폴더타입인 경우
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// .. 폴더는 제외
			if (!wcscmp(FindData.cFileName, L".."))
			{
				continue;
			}

			FindAssetPath(_FolderPath + FindData.cFileName + L"\\");
		}

		// 찾은 파일이 폴더타입이 아닌경우
		else
		{
			// 상대경로를 계산해서 저장
			wstring FilePath = _FolderPath + FindData.cFileName;
			wstring ContentPath = CPathMgr::GetInst()->GetContentPath();
			wstring RelativePath = FilePath.substr(ContentPath.length(), FilePath.length());
			m_vecAssetPath.push_back(RelativePath);

			// TEST : 메모리 프로파일링 asset 타입별로
			ASSET_TYPE type = GetAssetType(RelativePath);
			if (type != ASSET_TYPE::END)
				m_vecAssetPathByType[(UINT)GetAssetType(RelativePath)].push_back(RelativePath);
		}
	}

	FindClose(hHandle);
}

ASSET_TYPE ContentUI::GetAssetType(const wstring& _Path)
{
	wstring Ext = CPathMgr::GetInst()->GetFileExtension(_Path);

	if (L".mesh" == Ext)
		return MESH;
	if (L".mdat" == Ext)
		return MESH_DATA;
	if (L".bmp" == Ext || L".BMP" == Ext
		|| L".png" == Ext || L".PNG" == Ext
		|| L".jpg" == Ext || L".JPG" == Ext
		|| L".jpeg" == Ext || L".JPEG" == Ext
		|| L".tga" == Ext || L".TGA" == Ext
		|| L".dds" == Ext || L".DDS" == Ext)
		return TEXTURE;
	if (L".mp3" == Ext || L".MP3" == Ext
		|| L".ogg" == Ext || L".OGG" == Ext
		|| L".wav" == Ext || L".WAV" == Ext)
		return SOUND;
	if (L".pref" == Ext)
		return PREFAB;
	if (L".flip" == Ext)
		return FLIPBOOK;
	if (L".anim" == Ext)
		return ANIMATION;
	if (L".sprite" == Ext)
		return SPRITE;
	if (L".mtrl" == Ext)
		return MATERIAL;

	return END;
}

void ContentUI::ChangeName_ContentUI(DWORD_PTR _TreeNode)
{
	// 이름 설정
	if (ImGui::Selectable("Change Name", false, ImGuiSelectableFlags_DontClosePopups))
	{
		ImGui::OpenPopup("Name_Setting_popup_Asset");

		auto pNode = reinterpret_cast<TreeNode*>(_TreeNode);
		m_TargetAsset = reinterpret_cast<CAsset*>(pNode->GetData());
	}

	if (ImGui::BeginPopup("Name_Setting_popup_Asset"))
	{
		char szBuff[50]{};
		string strName = WStringToString(m_TargetAsset->GetKey());
		strcpy_s(szBuff, strName.c_str());
		if (ImGui::InputText("##AssetNameSet", szBuff, sizeof(szBuff), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			strName = szBuff;
			static wstring wstrName(L"");
			wstrName = wstring(strName.begin(), strName.end());

			// AssetMgr에 등록이 되어있는지 여부를 반환받음
			bool registered = CAssetMgr::GetInst()->ChangeAssetKey(m_TargetAsset, wstrName);
			assert(registered);

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

}

void ContentUI::CopyAsset(DWORD_PTR _TreeNode)
{
	if (ImGui::Selectable("Copy Asset", false, ImGuiSelectableFlags_DontClosePopups))
	{
		auto pNode = reinterpret_cast<TreeNode*>(_TreeNode);
		m_TargetAsset = reinterpret_cast<CAsset*>(pNode->GetData());

		m_TargetAsset = CAssetMgr::GetInst()->CopyAsset(m_TargetAsset);
	}
}

void ContentUI::SelectAsset(DWORD_PTR _TreeNode)
{
	auto pNode = reinterpret_cast<TreeNode*>(_TreeNode);
	m_TargetAsset = reinterpret_cast<CAsset*>(pNode->GetData());

	if (nullptr == m_TargetAsset)
		return;

	auto pInspector = static_cast<Inspector*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
	pInspector->SetTargetAsset(m_TargetAsset);
}
