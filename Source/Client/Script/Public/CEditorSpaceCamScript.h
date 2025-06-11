#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

enum ImGuiMouseButton_;

class CEditorSpaceCamScript :
	public CScript
{
private:
	float	m_CameraSpeed;

	Vec2	m_OriginMousePos;

	Vec2	m_MouseDelta;

	ImGuiMouseButton_ m_MouseBtn;

public:
	void Tick() override;
	void SetMouseDelta(float _x, float _y) { m_MouseDelta.x = _x;  m_MouseDelta.y = _y; }
	void SetMouseButton(ImGuiMouseButton_ _Btn) { m_MouseBtn = _Btn; }

private:
	//void CameraOrthgraphicMove();
	void CameraPerspectiveMove();

	void SaveComponent(FILE* _File) override {}
	void LoadComponent(FILE* _FILE) override {}


public:
	CLONE_DISABLE(CEditorSpaceCamScript);
	CEditorSpaceCamScript();
	~CEditorSpaceCamScript() override;
};
