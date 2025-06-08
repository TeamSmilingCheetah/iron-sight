#pragma once
#include "Client/UI/Public/Editor/EditorUI.h"

#include "Common/Ptr.h"

class CTexture;
class CGameObjectEx;

class TargetOBUI :
    public EditorUI
{
private:
	UINT			m_TargetID;
	Ptr<CTexture>	m_IDTex;
	Ptr<CTexture>	m_PositionTex;
	CGameObjectEx* m_PostObject;

	bool			m_ParentClick;

public:
	void Init();
	virtual void Render_Update() override;

	virtual void Activate() override {};
	virtual void Deactivate() override {};

public:
	TargetOBUI();
	~TargetOBUI();
};

