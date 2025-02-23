#include "pch.h"
#include "ContentUI.h"

#include <Engine/CTaskMgr.h>
#include <Engine/CAssetMgr.h>
#include <Engine/assets.h>


#include "CImGuiMgr.h"
#include "Inspector.h"
#include "TreeUI.h"

ContentUI::ContentUI()
    : EditorUI("Content")
{
    m_Tree = static_cast<TreeUI*>(AddChildUI(new TreeUI));
    m_Tree->SetName("ContentUI");
    m_Tree->ShowRoot(false);
    m_Tree->AddDynamicSelect(this, static_cast<EUI_DELEGATE_1>(&ContentUI::SelectAsset));

    // Content �����ȿ� �ִ� ��� ������ �޸𸮷� �ε�
    ReloadContent();

    // ���� CAssetMgr �� �ε��� ������ ������ Tree �� ���
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
            m_Tree->AddItem(pAssetNode, string(pair.first.begin(), pair.first.end()),
                            (DWORD_PTR)pair.second.Get());
        }
    }
}

void ContentUI::ReloadContent()
{
    // Content ���� �ȿ��ִ� ��� ������ ��θ� ã�Ƴ���.
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


    // ������ ���� ������ �����ϴ��� üũ
    for (UINT i = 0; i < END; ++i)
    {
        wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

        const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssets(
            static_cast<ASSET_TYPE>(i));

        for (const auto& pair : mapAsset)
        {
            // Engine Asset �� ���, ���������� �������� �����Ƿ�, �ǳʶڴ�.
            if (pair.second->IsEngineAsset())
                continue;

            // �޸𸮿� �ε��� ������ �������� ��� ���
            wstring strFilePath = ContentPath + pair.second->GetRelativePath();

            // ���������� ������(�����Ǿ�����), �޸𸮻� �ش� ���µ� ������Ų��.
            if (false == exists(strFilePath.c_str()))
            {
                MessageBox(nullptr, L"������ ���������� ������", L"���� ���� ���� ����", MB_OK);

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
        // ã�� ������ ����Ÿ���� ���
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // .. ������ ����
            if (!wcscmp(FindData.cFileName, L".."))
            {
                continue;
            }

            FindAssetPath(_FolderPath + FindData.cFileName + L"\\");
        }

        // ã�� ������ ����Ÿ���� �ƴѰ��
        else
        {
            // ����θ� ����ؼ� ����
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
