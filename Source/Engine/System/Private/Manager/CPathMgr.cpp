#include "pch.h"
#include "System/Public/Manager/CPathMgr.h"

CPathMgr::CPathMgr() = default;

CPathMgr::~CPathMgr() = default;

void CPathMgr::Init()
{
	// 현재 작업 디렉터리 확인
	wchar_t szExePath[MAX_PATH] = {};
	GetModuleFileNameW(nullptr, szExePath, MAX_PATH);
	path execute_path(szExePath);
	path bin_dir = execute_path.parent_path();

	m_BinPath = bin_dir.wstring() + L"\\";
	m_ContentPath = bin_dir.parent_path().wstring() + L"\\Content\\";
	m_SourcePath = bin_dir.parent_path().parent_path().wstring() + L"\\Source\\Engine\\";
	m_EffectsFilePath = m_SourcePath + L"Shader\\";
	m_ShaderBlobPath = m_ContentPath + L"Shader\\";
}

wstring CPathMgr::MakeFileName(const wstring& PName)
{
	wstring strName = PName;;

	int Len = static_cast<int>(strName.length());

	// 파일 명에 들어가면 안되는 문자들
	constexpr std::wstring_view badChars = L"/:*?\"<>|";

	for (int i = Len - 1; i >= 0; --i)
	{
		if (strName[i] == L'\\')
			break;

		if (badChars.find(strName[i]) != std::wstring::npos)  // `std::find()` 대체
			strName[i] = L'_';
	}

	return strName;
}

wstring CPathMgr::GetFileExtension(const wstring& PFilePath)
{
	path RelativePath = PFilePath;

	return RelativePath.extension();
}

wstring CPathMgr::GetKeyWithoutExtension(const wstring& PFilePath)
{
	size_t ExtPos = PFilePath.rfind(L".");
	if (ExtPos == wstring::npos)
		return L"";

	return PFilePath.substr(0, ExtPos);
}

wstring CPathMgr::GetFileStem(const wstring& PFilePath)
{
	path RelativePath = PFilePath;

	return RelativePath.stem();
}

wstring CPathMgr::GetRelativePath(const wstring& PFilePath)
{
	size_t FindPos = PFilePath.find(m_ContentPath);
	if (FindPos == wstring::npos)
		return L"";

	return PFilePath.substr(FindPos + m_ContentPath.length(), PFilePath.length());
}
