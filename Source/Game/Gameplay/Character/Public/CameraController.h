#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class CameraController :
	public CScript
{
	CGameObject* m_Player;
	class PlayerCharacter* m_PlayerScript;
	class InventoryController* m_InventoryScript;

	float m_CameraSpeed;

	float m_AccTime;
	float m_RecoilTime;
	float m_RecoilAmount_vertical;
	float m_RecoilAmount_horizontal;

	bool m_bCliked_First;
	bool m_bSearch;
	bool m_bSearchRecover;
	bool m_bRight;
	bool m_bShoulder;
	bool m_bShoulderRecover;
	bool m_bADS;
	bool m_bWasTPS;
	bool m_bChangeFocus;
	bool m_bChangeFOV;
	bool m_bTPS;


public:
	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	bool IsSearchRecover() { return m_bSearchRecover; }
	bool IsShoulder() { return m_bShoulder; }
	bool IsADS() { return m_bADS; }
	bool IsSearch() { return m_bSearch; }
	bool IsTPS() { return m_bTPS; }



private:
	void CameraOrthgraphicMove();
	void CameraPerspectiveMove();
	void ApplyZoom(bool _IsADS);

public:
	CLONE(CameraController);
	CameraController();
	~CameraController() override;
};
