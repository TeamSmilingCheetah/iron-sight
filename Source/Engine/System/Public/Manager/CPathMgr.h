#pragma once

class CPathMgr
    : public singleton<CPathMgr>
{
    SINGLE(CPathMgr);
    wstring m_ContentPath;
    wstring m_BinPath;
	wstring m_SourcePath;

public:
    const wstring& GetContentPath() { return m_ContentPath; }
    const wstring& GetBinPath() { return m_BinPath; }
	const wstring& GetSrcPath() { return m_SourcePath; }
    wstring GetRelativePath(const wstring& _FilePath);
    void Init();

	wstring MakeFileName(const wstring& _Name);
	wstring GetFileExtension(const wstring& _FilePath);
	wstring GetFileStem(const wstring& _FilePath);
	wstring GetKeyWithoutExtension(const wstring& _FilePath);
};
