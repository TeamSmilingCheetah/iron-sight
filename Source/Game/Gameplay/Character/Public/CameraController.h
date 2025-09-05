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

	Vec3 m_ObstacleAdjustPos;
	Vec3 m_ObstaclePos;
	Vec3 m_ObstacleScale;
	float m_OriginDistance;
	float m_RayDistance;

	float m_CameraSpeed;
	float m_CameraYOffset;
	float m_CurClipAccTime;
	float m_RecoilTime;
	float m_ObstacleResetTime;
	float m_RecoilAmount_vertical;
	float m_RecoilAmount_horizontal;
	float m_TargetRecoilRotX;
	float m_TargetRecoilRotY;

	float m_LateralOffset;
	float m_ObjectiveLateralOff;

	float m_AdjustNormalDistance;
	float m_AdjustNormalHeight;
	float m_AdjustFinalDistance;
	float m_AdjustFinalHeight;

	float m_ObjectiveShoulderDistance;
	float m_ObjectiveShoulderHeight;

	float m_LastVerticalVelocity;
	float m_VerticalSmoothTime;
	float m_CurrentVerticalVelocity;

	// 반동 관련
	int m_CurStep;
	int m_CurSingleStep;
	float m_StepTimer;
	float m_SingleTimer;
	bool  m_SingleFire;
	bool  m_SingleTargetValid;

	float m_SingleTargetCameraRotX;
	float m_SingleTargetPlayerRotY;

	UINT m_CameraFlag;
	bool m_bLevelChanged;
	float m_LevelChangeTime;

public:
	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	void BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;
	void Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;
	void EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;

	void SetFlag(CAM_FLAG _flag, bool _value);
	bool GetFlag(CAM_FLAG _flag) const { return (m_CameraFlag & _flag) != 0; }

	void ChangePS(bool _bTPS);

	void SetSingleRecoilOn(){
		m_CameraFlag |= SINGLE_RECOIL_UPDATE;
		m_SingleTimer = 0.f;
		m_SingleTargetValid = false;
	}
	void SetSingleFireOn() {
		if(m_CameraFlag & SINGLE_RECOIL_UPDATE)
			m_SingleFire = true;
	}

private:
	void CameraOrthgraphicMove();
	void CameraPerspectiveMove();
	void CameraDebugMove();

	void UpdateTPSCameraAdjustments();

	void ApplyZoom(bool _IsADS);

	float ApplyRecoilNoise(float _value, float _noiseScale);
	void ResetRecoilPattern();
	void UpdateRecoil();
	void UpdateSingleRecoil();
	
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
