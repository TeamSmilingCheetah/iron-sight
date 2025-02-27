#pragma once
#include "Runtime/Public/Component/Script/CScript.h"

class CCameraScript :
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
	CLONE(CCameraScript);
	CCameraScript();
	~CCameraScript() override;
};
