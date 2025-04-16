#include "pch.h"
#include "Client/UI/Public/Editor/ContentUI.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CTaskMgr.h"
#include "Client/System/Public/CImGuiMgr.h"
#include "Client/UI/Public/Editor/Inspector.h"
#include "Client/UI/Public/Editor/TreeUI.h"

class Inspector;

ContentUI::ContentUI()
	: EditorUI("Content")
{
	m_Tree = static_cast<TreeUI*>(AddChildUI(new TreeUI));
	m_Tree->SetName("ContentUI");
	m_Tree->ShowRoot(false);
	m_Tree->AddDynamicSelect(this, static_cast<EUI_DELEGATE_1>(&ContentUI::SelectAsset));

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
	FindAssetPath(CPathMgr::GetInst()->GetContentPath());

	for (size_t i = 0; i < m_vecAssetPath.size(); ++i)
	{
		ASSET_TYPE Type = GetAssetType(m_vecAssetPath[i]);

		switch (Type)
		{
		case MESH:
			CAssetMgr::GetInst()->Load<CMesh>(m_vecAssetPath[i], m_vecAssetPath[i]);
			break;
		case MESH_DATA:
			CAssetMgr::GetInst()->Load<CMeshData>(m_vecAssetPath[i], m_vecAssetPath[i]);
			break;
		case TEXTURE:
			CAssetMgr::GetInst()->Load<CTexture>(m_vecAssetPath[i], m_vecAssetPath[i]);
			break;
		case SOUND:
			CAssetMgr::GetInst()->Load<CSound>(m_vecAssetPath[i], m_vecAssetPath[i]);
			break;
		case PREFAB:
			CAssetMgr::GetInst()->Load<CPrefab>(m_vecAssetPath[i], m_vecAssetPath[i]);
			break;
		case FLIPBOOK:
			CAssetMgr::GetInst()->Load<CFlipbook>(m_vecAssetPath[i], m_vecAssetPath[i]);
			break;
		case SPRITE:
			CAssetMgr::GetInst()->Load<CSprite>(m_vecAssetPath[i], m_vecAssetPath[i]);
			break;
		case MATERIAL:
			CAssetMgr::GetInst()->Load<CMaterial>(m_vecAssetPath[i], m_vecAssetPath[i]);
			break;
		}
	}

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
		}
	}

	FindClose(hHandle);
}

ASSET_TYPE ContentUI::GetAssetType(const wstring& _Path)
{
	path RelativePath = _Path;

	//path FileName = RelativePath.stem();
	path Ext = RelativePath.extension();

	if (".mesh" == Ext)
		return MESH;
	if (".mdat" == Ext)
		return MESH_DATA;
	if (".bmp" == Ext || ".BMP" == Ext
		|| ".png" == Ext || ".PNG" == Ext
		|| ".jpg" == Ext || ".JPG" == Ext
		|| ".jpeg" == Ext || ".JPEG" == Ext
		|| ".tga" == Ext || ".TGA" == Ext
		|| ".dds" == Ext || ".DDS" == Ext)
		return TEXTURE;
	if (".mp3" == Ext || ".MP3" == Ext
		|| ".ogg" == Ext || ".OGG" == Ext
		|| ".wav" == Ext || ".WAV" == Ext)
		return SOUND;
	if (".pref" == Ext)
		return PREFAB;
	if (".flip" == Ext)
		return FLIPBOOK;
	if (".sprite" == Ext)
		return SPRITE;
	if (".mtrl" == Ext)
		return MATERIAL;

	return END;
}

void ContentUI::SelectAsset(DWORD_PTR _TreeNode)
{
	auto pNode = (TreeNode*)_TreeNode;
	Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();

	if (nullptr == pAsset)
		return;

	auto pInspector = static_cast<Inspector*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
	pInspector->SetTargetAsset(pAsset);
}
