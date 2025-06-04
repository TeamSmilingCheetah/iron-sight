#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class CameraController :
	public CScript
{
	CGameObject* m_Player;
	class PlayerCharacter* m_PlayerScript;
	class InventoryController* m_InventoryScript;

	Vec3 m_CameraPos;
	Vec3 m_CameraRot;
	Vec3 m_PlayerPos;
	Vec3 m_PlayerRot;


	float m_CameraSpeed;
	float m_CameraYOffset;

	float m_AccTime;
	float m_RecoilTime;
	float m_RecoilAmount_vertical;
	float m_RecoilAmount_horizontal;

	float m_LateralOffset;
	float m_ObjectiveLateralOff;

	float m_AdjustNormalDistance;
	float m_AdjustNormalHeight;
	float m_AdjustFinalDistance;
	float m_AdjustFinalHeight;

	float m_ObjectiveShoulderDistance;
	float m_ObjectiveShoulderHeight;

	UINT m_CameraFlag;


public:
	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	void SetFlag(CAM_FLAG _flag, bool _value);
	bool GetFlag(CAM_FLAG _flag) const { return (m_CameraFlag & _flag) != 0; }

	void ChangePS(bool _bTPS);

private:
	void CameraOrthgraphicMove();
	void CameraPerspectiveMove();

	void UpdateTPSCameraAdjustments();

	void ApplyZoom(bool _IsADS);
	void ApplyRecoil();

	void HandleRightClickInput();

	void UpdateShoulderMode();
	void UpdateSearchMode();
	void UpdateShoulderRecover();

	void UpdateTPSLean();
	void UpdateFPSLean();

	void UpdateStance();


public:
	CLONE(CameraController);
	CameraController();
	~CameraController() override;
};
