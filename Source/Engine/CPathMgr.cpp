#include "pch.h"
#include "CPathMgr.h"

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

wstring CPathMgr::GetRelativePath(const wstring& _FilePath)
{
    size_t FindPos = _FilePath.find(m_ContentPath);
    if (FindPos == wstring::npos)
        return L"";

    return _FilePath.substr(FindPos + m_ContentPath.length(), _FilePath.length());
}
