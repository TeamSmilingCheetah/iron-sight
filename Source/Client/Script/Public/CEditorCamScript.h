#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class CEditorCamScript :
	public CScript
{
private:
	float m_CameraSpeed;

public:
	void Tick() override;

private:
	void CameraOrthgraphicMove();
	void CameraPerspectiveMove();

	void SaveComponent(FILE* _File) override {}
	void LoadComponent(FILE* _FILE) override {}

public:
	CLONE_DISABLE(CEditorCamScript);
	CEditorCamScript();
	~CEditorCamScript() override;
};
