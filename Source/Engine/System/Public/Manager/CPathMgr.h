#pragma once

/**
 * @brief 경로 관리 매니저 클래스
 */
class CPathMgr
    : public singleton<CPathMgr>
{
    SINGLE(CPathMgr);
    wstring m_ContentPath;
    wstring m_BinPath;
	wstring m_SourcePath;
	wstring m_EffectsFilePath;
	wstring m_ShaderBlobPath;

public:
	void Init();
	wstring MakeFileName(const wstring& PName);
	wstring GetRelativePath(const wstring& PFilePath);
	wstring GetFileExtension(const wstring& PFilePath);
	wstring GetFileStem(const wstring& PFilePath);
	wstring GetKeyWithoutExtension(const wstring& PFilePath);

	// Getter & Setter
    const wstring& GetContentPath() { return m_ContentPath; }
    const wstring& GetBinPath() { return m_BinPath; }
	const wstring& GetSrcPath() { return m_SourcePath; }
	const wstring& GetEffectsFilePath() { return m_EffectsFilePath; }
	const wstring& GetShaderBlobPath() { return m_ShaderBlobPath; }
};
