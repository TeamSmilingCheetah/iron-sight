#pragma once

class EditorUI;

class CImGuiMgr
	: public singleton<CImGuiMgr>
{
	SINGLE(CImGuiMgr)
	map
	<
		string
		,
		EditorUI*>
	m_mapUI;
	HANDLE m_hNotify;

public:
	EditorUI* FindUI(const string& _ID);

	int Init();
	void Progress();

private:
	void CreateEditorUI();
	void ObserveContent();

	void Tick();
	void Render();
};
