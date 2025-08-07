#pragma once
#include "Common/singleton.h"

class CGameObject;
class CameraController;
class KillinfoUIScript;
class CameraEffect;

class CGameMgr
	: public singleton<CGameMgr>
{
	SINGLE(CGameMgr)

private:
	CGameObject*	m_Player;
	CGameObject*	m_MainCamera;

	// UI
	CGameObject*	m_InventoryCanvasUI;	// 인벤토리 UI
	CGameObject*	m_CardinalImageUI;		// 방위 UI
	CGameObject*	m_HPUI;					// HP UI
	CGameObject*	m_ItemUseUI;			// 아이템 사용 UI
	CGameObject*	m_ReloadUI;				// 재장전 UI


	CameraController*		m_CamScript;
	KillinfoUIScript*		m_KillinfoScript;
	CameraEffect*			m_CameraEffect;


public:
	CGameObject* GetPlayer() const { return m_Player; }
	CGameObject* GetMainCam() const { return m_MainCamera; }

	CGameObject* GetInventoryCanvasUI() const { return m_InventoryCanvasUI; }
	CGameObject* GetCardinalUI() const { return m_CardinalImageUI; }
	CGameObject* GetHPUI() const { return m_HPUI; }
	CGameObject* GetItemUseUI() const { return m_ItemUseUI; }
	CGameObject* GetReloadUI() const { return m_ReloadUI; }

	CameraController* GetCamScript() const { return m_CamScript; }
	KillinfoUIScript* GetKillInfoUI() const { return m_KillinfoScript; }
	CameraEffect* GetCamEffect() const { return m_CameraEffect; }

public:
	void SetItemUseUITime(float _RemainTime, float _TotalTime);
	void UpdateCardinalUI(float _RotationY);
	void SetHPUI(float _curHP, float _semimaxHP, float _maxHP, float _curBoost, float _maxBoost, float _healAmount = 0.f);

public:
	int Init();
	void Begin();
};
