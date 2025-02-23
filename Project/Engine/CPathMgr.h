#pragma once


class CPathMgr
    : public singleton<CPathMgr>
{
    SINGLE(CPathMgr);
    wstring m_ContentPath;
    wstring m_BinPath;

public:
    const wstring& GetContentPath() { return m_ContentPath; }
    const wstring& GetBinPath() { return m_BinPath; }
    wstring GetRelativePath(const wstring& _FilePath);
    void Init();
};
