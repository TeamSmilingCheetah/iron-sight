#pragma once
#include "Client/UI/Public/Editor/EditorUI.h"

class ComponentUI :
	public EditorUI
{
	CGameObject* m_TargetObject;
	const COMPONENT_TYPE m_Type;

public:
	void SetTargetObject(CGameObject* _Target);
	CGameObject* GetTargetObject() { return m_TargetObject; }

protected:
	void ComponentTitle(const string& _title);

	virtual void DeleteComponent(COMPONENT_TYPE _Type);

public:
	ComponentUI(const string& _ID, COMPONENT_TYPE _Type);
	~ComponentUI() override;
};
