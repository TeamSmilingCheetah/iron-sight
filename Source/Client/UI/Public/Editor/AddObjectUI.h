#pragma once
#include "EditorUI.h"

class AddObjectUI :
	public EditorUI
{
public:
	CGameObject* m_Object;
	int          m_Idx;
	bool         m_MoveWithChild;

private:
	virtual void Render_Update() override;

	//void SelectPrefab(DWORD_PTR _ListUI, DWORD_PTR _SelectString);

public:
	AddObjectUI();
	~AddObjectUI();
};
