#include "pch.h"
#include "Client/UI/Public/Editor/ContentUI.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CTaskMgr.h"
#include "Client/System/Public/CImGuiMgr.h"
#include "Client/UI/Public/Editor/Inspector.h"
#include "Client/UI/Public/Editor/TreeUI.h"

#include "Engine/Core/Public/Engine.h"

tFSNode* ContentUI::m_rootAssetFileSystem = new tFSNode(L"Content");

ContentUI::ContentUI()
	: EditorUI("Content")
{
	m_Tree = static_cast<TreeUI*>(AddChildUI(new TreeUI));
	m_Tree->SetName("ContentUI");
	m_Tree->ShowRoot(false);
	m_Tree->RightOption(true, this);

	m_Tree->AddDynamicSelect(this, static_cast<EUI_DELEGATE_1>(&ContentUI::SelectAsset));
	m_Tree->AddRightItemDelegate((EUI_DELEGATE_1)&ContentUI::SelectAsset);
	m_Tree->AddRightItemDelegate((EUI_DELEGATE_1)&ContentUI::ChangeName_ContentUI);
	m_Tree->AddRightItemDelegate((EUI_DELEGATE_1)&ContentUI::CopyAsset);


	// Content 폴더안에 있는 모든 에셋을 메모리로 로딩
	ReloadContent();

	// 현재 CAssetMgr 에 로딩된 에셋의 정보를 Tree 에 출력
	RenewContent();
}

ContentUI::~ContentUI()
{
	delete m_rootAssetFileSystem;
}

void ContentUI::Render_Update()
{
}

void ContentUI::Reset()
{
	ReloadContent();

	RenewContent();
}

void ContentUI::RenewContent()
{
	m_Tree->Clear();

	ConstructFileSystem(m_rootAssetFileSystem, nullptr);
}

void ContentUI::ReloadContent()
{
	// FileSystem Clear
	delete m_rootAssetFileSystem;
	m_rootAssetFileSystem = new tFSNode(L"Content");

	FindAssetPath(CPathMgr::GetInst()->GetContentPath(), m_rootAssetFileSystem);

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

void ContentUI::FindAssetPath(const wstring& _FolderPath, tFSNode* _ParentNode)
{
	wstring Path = _FolderPath + L"*.*";

	WIN32_FIND_DATA FindData = {};

	HANDLE hHandle = FindFirstFile(Path.c_str(), &FindData);

	tFSNode* ChildNode = nullptr;

	while (FindNextFile(hHandle, &FindData))
	{
		// 찾은 파일이 폴더타입인 경우
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// ".." 폴더는 제외
			if (!wcscmp(FindData.cFileName, L".."))
			{
				continue;
			}

			// TEST : 파일 탐색기
			ChildNode = new tFSNode;
			ChildNode->isFolder = true;
			ChildNode->Name = FindData.cFileName;
			ChildNode->Parent = _ParentNode;
			_ParentNode->vecChildren.push_back(ChildNode);

			FindAssetPath(_FolderPath + FindData.cFileName + L"\\", ChildNode);
		}

		// 찾은 파일이 폴더타입이 아닌경우
		else
		{
			// TEST : 파일 탐색기
			ChildNode = new tFSNode;
			ChildNode->isFolder = false;
			ChildNode->Name = FindData.cFileName;
			ChildNode->Parent = _ParentNode;
			_ParentNode->vecChildren.push_back(ChildNode);
		}
	}

	// 정렬
	std::sort(_ParentNode->vecChildren.begin(), _ParentNode->vecChildren.end(), [](const tFSNode* _lhs, const tFSNode* _rhs)
		{
			if (_lhs->isFolder != _rhs->isFolder)
				return _lhs->isFolder;

			return ToLower(_lhs->Name) < ToLower(_rhs->Name);
		});

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
		tFSNode* pFSNode = reinterpret_cast<tFSNode*>(pNode->GetData());

		m_TargetAsset = pFSNode->Asset;
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
		tFSNode* pFSNode = reinterpret_cast<tFSNode*>(pNode->GetData());

		m_TargetAsset = pFSNode->Asset;
		m_TargetAsset = CAssetMgr::GetInst()->CopyAsset(m_TargetAsset);

		m_TargetAsset->Save(m_TargetAsset->GetRelativePath());
	}
}

void ContentUI::ConstructFileSystem(tFSNode* _CurFSNode, TreeNode* _CurTreeNode)
{
	TreeNode* CurTreeNode = m_Tree->AddItem(_CurTreeNode, WStringToString(_CurFSNode->Name), reinterpret_cast<DWORD_PTR>(_CurFSNode), _CurFSNode->isFolder);

	for (auto child : _CurFSNode->vecChildren)
	{
		ConstructFileSystem(child, CurTreeNode);
	}
}

void ContentUI::SelectAsset(DWORD_PTR _TreeNode)
{
	auto pNode = reinterpret_cast<TreeNode*>(_TreeNode);
	tFSNode* origFSNode = reinterpret_cast<tFSNode*>(pNode->GetData());

	if (nullptr == origFSNode || origFSNode->isFolder)
		return;

	m_TargetAsset = LoadAsset(origFSNode);

	// Inspector Target Asset으로 등록
	auto pInspector = static_cast<Inspector*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
	pInspector->SetTargetAsset(m_TargetAsset);
}

Ptr<CAsset> ContentUI::LoadAsset(tFSNode* _FSNode)
{
	if (nullptr == _FSNode || _FSNode->isFolder)
		return nullptr;

	Ptr<CAsset> pAsset = _FSNode->Asset;

	if (pAsset == nullptr)
	{
		tFSNode* curFSNode = _FSNode;

		// Relative Path 계산
		wstring RelativeFilePath = curFSNode->Name;

		while (curFSNode->Parent != m_rootAssetFileSystem)
		{
			RelativeFilePath = curFSNode->Parent->Name + L"\\" + RelativeFilePath;
			curFSNode = curFSNode->Parent;
		}

		// Type에 따라 Asset Load 함
		ASSET_TYPE Type = GetAssetType(RelativeFilePath);

		switch (Type)
		{
		case MESH:
			pAsset = static_cast<CAsset*>(CAssetMgr::GetInst()->Load<CMesh>(RelativeFilePath, RelativeFilePath).Get());
			break;
		case MESH_DATA:
			pAsset = static_cast<CAsset*>(CAssetMgr::GetInst()->Load<CMeshData>(RelativeFilePath, RelativeFilePath).Get());
			break;
		case TEXTURE:
			pAsset = static_cast<CAsset*>(CAssetMgr::GetInst()->Load<CTexture>(RelativeFilePath, RelativeFilePath).Get());
			break;
		case SOUND:
			pAsset = static_cast<CAsset*>(CAssetMgr::GetInst()->Load<CSound>(RelativeFilePath, RelativeFilePath).Get());
			break;
		case PREFAB:
			pAsset = static_cast<CAsset*>(CAssetMgr::GetInst()->Load<CPrefab>(RelativeFilePath, RelativeFilePath).Get());
			break;
		case FLIPBOOK:
			pAsset = static_cast<CAsset*>(CAssetMgr::GetInst()->Load<CFlipbook>(RelativeFilePath, RelativeFilePath).Get());
			break;
		case ANIMATION:
			pAsset = static_cast<CAsset*>(CAssetMgr::GetInst()->Load<CAnimation>(RelativeFilePath, RelativeFilePath).Get());
			break;
		case SPRITE:
			pAsset = static_cast<CAsset*>(CAssetMgr::GetInst()->Load<CSprite>(RelativeFilePath, RelativeFilePath).Get());
			break;
		case MATERIAL:
			pAsset = static_cast<CAsset*>(CAssetMgr::GetInst()->Load<CMaterial>(RelativeFilePath, RelativeFilePath).Get());
			break;
		}

		_FSNode->Asset = pAsset;
	}

	return pAsset;
}
