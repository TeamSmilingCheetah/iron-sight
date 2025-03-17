#pragma once

#include "Client/UI/Public/Editor/EditorUI.h"

class CPrefab;

class PrefabEditor :
	public EditorUI
{
private:
	Ptr<CPrefab> m_Prefab;
	CGameObject* m_ProtoObject;

	char            m_Buffer[128];

public:
	virtual void Render_Update() override;
	void Init();

public:
	void LoadFBX(wstring _Path);;
	void IsRender();

private:
	void SelectComponent(DWORD_PTR _ListUI, DWORD_PTR _SelectString);
	void DeleteComponent(DWORD_PTR _ListUI, DWORD_PTR _SelectString);


public:
	PrefabEditor();
	~PrefabEditor();

	friend class ObjectEditor;
};

