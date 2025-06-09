#include "pch.h"
#include "System/Public/Manager/CPathMgr.h"

CPathMgr::CPathMgr()
{
}

CPathMgr::~CPathMgr()
{
}

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
}

wstring CPathMgr::MakeFileName(const wstring& _Name)
{
	wstring strName = _Name;;

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

wstring CPathMgr::GetFileExtension(const wstring& _FilePath)
{
	path RelativePath = _FilePath;

	return RelativePath.extension();
}

wstring CPathMgr::GetKeyWithoutExtension(const wstring& _FilePath)
{
	size_t ExtPos = _FilePath.rfind(L".");
	if (ExtPos == wstring::npos)
		return L"";

	return _FilePath.substr(0, ExtPos);
}

wstring CPathMgr::GetFileStem(const wstring& _FilePath)
{
	path RelativePath = _FilePath;

	return RelativePath.stem();
}

wstring CPathMgr::GetRelativePath(const wstring& _FilePath)
{
	size_t FindPos = _FilePath.find(m_ContentPath);
	if (FindPos == wstring::npos)
		return L"";

	return _FilePath.substr(FindPos + m_ContentPath.length(), _FilePath.length());
}
