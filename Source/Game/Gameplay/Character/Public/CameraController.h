#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class CameraController :
	public CScript
{
	float m_CameraSpeed;

public:
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

private:
	void CameraOrthgraphicMove();
	void CameraPerspectiveMove();

public:
	CLONE(CameraController);
	CameraController();
	~CameraController() override;
};
