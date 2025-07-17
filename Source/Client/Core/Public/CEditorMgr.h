#pragma once
#include <Common/singleton.h>

class CGameObjectEx;
class CMRT;
class CEditorSpaceCamScript;

class CEditorMgr :
	public singleton<CEditorMgr>
{
	SINGLE(CEditorMgr);

private:
	vector<CGameObjectEx*>	m_vecEditorObj;

	// 에디터에서 애니메이션을 확인하기 위한 렌더링 공간
	bool					m_RenderEditorSpace;
	vector<CGameObjectEx*>	m_vecEditorSpaceObj;
	CGameObjectEx*			m_EditorSpaceCam;
	CGameObjectEx*			m_Light;
	CGameObjectEx*			m_Origin;
	CMRT*					m_EditorSpaceRT;

	CEditorSpaceCamScript*	m_EditorSpaceCamScript;

public:
	void Init();
	void Progress();

	void Render_Init();
	void Render_Deferred();
	void Render_Light();
	void Render_Merge();
	void Render_Clear();

	void CreateEditorObj(CGameObjectEx* _EditorObj);
	void CreateEditorSpaceObj(CGameObjectEx* _EditorSpaceObj);

	void SetEditorSpaceRender(bool _b) { m_RenderEditorSpace = _b; }

	CEditorSpaceCamScript* GetEditorSpaceCamScript() { return m_EditorSpaceCamScript; }
};
