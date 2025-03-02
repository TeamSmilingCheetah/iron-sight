#pragma once
#include "Client/UI/Public/Editor/EditorUI.h"
#include "Client/UI/Public/SpriteEditor/SpriteEditor.h"

class SE_Sub :
	public EditorUI
{
	SpriteEditor* m_Owner;

public:
	SpriteEditor* GetOwner() { return m_Owner; }
	class SE_Detail* GetDetail() { return m_Owner->GetDetail(); }
	class SE_AtlasView* GetAtlasView() { return m_Owner->GetAtlasView(); }


	SE_Sub(const string& _ID);
	~SE_Sub() override;

	friend class SpriteEditor;
};
