#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class CEditorSpaceCamScript :
	public CScript
{
private:
	float	m_CameraSpeed;

	Vec2	m_OriginMousePos;

public:
	void Tick() override;

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
