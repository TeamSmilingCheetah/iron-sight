#pragma once

class CUI;

class CUIMgr
	: public singleton<CUIMgr>
{
	SINGLE(CUIMgr);

private:
	vector<CUI*>		m_vecUI;

	CUI*				m_HoverUI;
	CUI*				m_FocusUI;		// 실제로 focus된 UI
	CUI*				m_FocusCanvas;	// focusUI를 들고 있는 Canvas

	CUI*				m_DragUI;

	vector<PayLoad>		m_vecPayLoad;	// Drag된 UI의 스크립트에서 전달하려는 Payload 모음

public:
	int GetCanvasCount() const { return static_cast<int>(m_vecUI.size()); }
	void RegisterUI(CUI* _UI);
	void SwapPriority(CUI* _UI, int Priority);

	// Event 함수 호출
	void OnMouseClick();
	void OnMouseHover();
	void OnMouseBeginDrag();
	void OnMouseDrop();

public:
	void Tick();

private:
	CUI* CheckMouseHover(CUI* _UI);
	void ChangeFocus(CUI* _CanvasUI, CUI* _FocusUI);
};
