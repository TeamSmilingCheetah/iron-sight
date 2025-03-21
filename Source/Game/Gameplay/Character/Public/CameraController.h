#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class CameraController :
	public CScript
{
	float m_CameraSpeed;
	bool m_bSearchRecover;
	bool m_bRight;
	bool m_bZoom;
	bool m_bZoomRecover;
	bool m_bChangeFocus;
	

public:
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	bool IsSearchRecover() { return m_bSearchRecover; }

private:
	void CameraOrthgraphicMove();
	void CameraPerspectiveMove();

public:
	CLONE(CameraController);
	CameraController();
	~CameraController() override;
};
