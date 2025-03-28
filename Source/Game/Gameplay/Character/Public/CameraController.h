#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class CameraController :
	public CScript
{
	float m_CameraSpeed;

	float m_RecoilTime;
	float m_RecoilAmount_vertical;
	float m_RecoilAmount_horizontal;

	bool m_bSearch;
	bool m_bSearchRecover;
	bool m_bRight;
	bool m_bShoulder;
	bool m_bShoulderRecover;
	bool m_bADS;
	bool m_bChangeFocus;

public:
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	bool IsSearchRecover() { return m_bSearchRecover; }
	bool IsShoulder() { return m_bShoulder; }
	bool IsADS() { return m_bADS; }
	bool IsSearch() { return m_bSearch; }

private:
	void CameraOrthgraphicMove();
	void CameraPerspectiveMove();

public:
	CLONE(CameraController);
	CameraController();
	~CameraController() override;
};
