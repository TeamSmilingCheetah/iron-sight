#pragma once
#include "Common/singleton.h"

class CGameObject;
class CameraController;
class KillinfoUIScript;
class CameraEffect;
class PlayerCharacter;
class TestFadeInOutReset;
class PlayerRevive;

class CGameMgr
	: public singleton<CGameMgr>
{
	SINGLE(CGameMgr)

private:
	CGameObject*	m_Player;
	CGameObject*	m_MainCamera;

	// UI
	CGameObject*	m_PauseCanvasUI;		// Pause UI
	CGameObject*	m_OptionCanvasUI;		// 옵션 UI
	CGameObject*	m_InventoryCanvasUI;	// 인벤토리 UI
	CGameObject*	m_CardinalCanvasUI;		// 방위 UI
	CGameObject*	m_MainCanvasUI;			// Main UI


	CGameObject*	m_CardinalImageUI;		// 방위 이미지 UI
	CGameObject*	m_HPUI;					// HP UI
	CGameObject*	m_TimerUI;				// 아이템 사용 UI
	CGameObject*	m_RestartUI;			// 재시작 UI
	
	// Event
	TestFadeInOutReset* m_FadeInOutEvent;			// Player Reset
	PlayerRevive*		m_PlayerReviveEvent;		// Player Revive

	// Script
	CameraController*		m_CamScript;
	KillinfoUIScript*		m_KillinfoScript;
	CameraEffect*			m_CameraEffect;
	PlayerCharacter*		m_PlayerScript;

public:
	CGameObject* GetPlayer() const { return m_Player; }
	CGameObject* GetMainCamera() const { return m_MainCamera; }

	CGameObject* GetInventoryCanvasUI() const { return m_InventoryCanvasUI; }
	CGameObject* GetPauseCanvasUI() const { return m_PauseCanvasUI; }
	CGameObject* GetOptionCanvasUI() const {return m_OptionCanvasUI; }
	CGameObject* GetCardinalUI() const { return m_CardinalImageUI; }
	CGameObject* GetHPUI() const { return m_HPUI; }
	CGameObject* GetTimerUI() const { return m_TimerUI; }
	CGameObject* GetRestartUI() const { return m_RestartUI; }

	CameraController* GetCamScript() const { return m_CamScript; }
	KillinfoUIScript* GetKillInfoUI() const { return m_KillinfoScript; }
	CameraEffect* GetCamEffect() const { return m_CameraEffect; }
	PlayerCharacter* GetPlayerScript() const { return m_PlayerScript; }


	TestFadeInOutReset* GetFadeInOut() const { return m_FadeInOutEvent; }
	PlayerRevive* GetPlayerRevive() const { return m_PlayerReviveEvent; }

public:
	// Game Callback Function -> 위치 고려 필요성
	void ResumeGame();
	void RestartGame();
	void OpenOption();
	void ExitGame();

	void UpSensi();
	void DownSensi();
	void ExitOption();

public:
	void SetTimerUI(float _RemainTime, float _TotalTime);
	void UpdateCardinalUI(float _RotationY);
	void SetHPUI(float _curHP, float _semimaxHP, float _maxHP, float _curBoost, float _maxBoost, float _healAmount = 0.f);

public:
	int Init();
	void Begin();
};
