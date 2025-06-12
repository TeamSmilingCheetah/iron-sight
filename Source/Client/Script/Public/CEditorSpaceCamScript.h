#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

enum ImGuiMouseButton_;

class CEditorSpaceCamScript :
	public CScript
{
private:
	Vec2				m_MouseDelta;	// 전 프레임에 비한 변화량
	ImGuiMouseButton_	m_MouseBtn;		// 입력키에 따라 조건 분기

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
