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
    // ���� �۾� ���͸��� �˾Ƴ���. (�����Ų *.exe ���� ��ġ OR ������Ʈ �۾����͸� ��ġ)
    wchar_t szCurDir[256];
    GetCurrentDirectory(256, szCurDir);

    int Len = wcslen(szCurDir);

    for (int i = Len - 1; 0 < i; --i)
    {
        if (szCurDir[i] == L'\\')
        {
            szCurDir[i + 1] = L'\0';
            break;
        }
    }

    m_BinPath = szCurDir;
    m_ContentPath = m_BinPath + L"Content\\";
    m_BinPath += L"bin\\";
}


wstring CPathMgr::GetRelativePath(const wstring& _FilePath)
{
    size_t FindPos = _FilePath.find(m_ContentPath);
    if (FindPos == wstring::npos)
        return L"";

    return _FilePath.substr(FindPos + m_ContentPath.length(), _FilePath.length());
}
