#include "pch.h"
#include "System/Public/Asset/Texture/CFlipbook.h"
#include "System/Public/Manager/CPathMgr.h"

CFlipbook::CFlipbook(bool _bEngineRes)
	: FAsset(FLIPBOOK, _bEngineRes)
{
}

CFlipbook::~CFlipbook()
{
}


int CFlipbook::Save(const wstring& _RelativePath)
{
	wstring strRelativePath = CPathMgr::GetInst()->MakeFileName(_RelativePath);
	SetRelativePath(strRelativePath);

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + strRelativePath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");
	assert(pFile);

	// 스프라이트 개수
	size_t SpriteCount = m_vecSprite.size();
	fwrite(&SpriteCount, sizeof(size_t), 1, pFile);

	// 각 스프라이트 정보
	for (size_t i = 0; i < SpriteCount; ++i)
	{
		SaveAssetRef(m_vecSprite[i], pFile);
	}

	fclose(pFile);

	wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
	int FindIdx = static_cast<int>(strFilePath.find(strContentPath));
	if (FindIdx != -1)
	{
		wstring RelativePath = strFilePath.substr(strContentPath.length(), strFilePath.length());
		SetRelativePath(RelativePath);
	}

	return S_OK;
}

int CFlipbook::Load(const wstring& _FilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");
	assert(pFile);

	// 스프라이트 개수
	size_t SpriteCount = 0;
	fread(&SpriteCount, sizeof(size_t), 1, pFile);
	m_vecSprite.resize(SpriteCount);

	// 각 스프라이트 정보
	for (size_t i = 0; i < SpriteCount; ++i)
	{
		LoadAssetRef(m_vecSprite[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}
