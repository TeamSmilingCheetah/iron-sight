#include "pch.h"
#include "Game/System/Public/CGameMgr.h"

#include "Engine/System/Public/Manager/CStateMgr.h"
#include "Engine/System/Public/Manager/CScriptMgr.h"

#include "Game/Gameplay/states.h"
#include "Game/Gameplay/scripts.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"

CGameMgr::CGameMgr()
{

}

CGameMgr::~CGameMgr()
{

}


// _RemainTime = 아이템 사용까지 남은 시간
// _TotalTIme = 아이템 사용에 걸리는 총 시간
void CGameMgr::SetItemUseUITime(float _RemainTime, float _TotalTime)
{
	m_ItemUseUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, 1.f - _RemainTime / _TotalTime);

	// 남은 시간 글씨 출력
	wchar_t text[4]{};	// 3글자 출력
	swprintf_s(text, L"%.1f", _RemainTime);
	m_ItemUseUI->UI()->GetTextInfoRef()[0].Text = text;
}

void CGameMgr::UpdateCardinalUI(float _RotationY)
{
	CGameMgr::GetInst()->GetCardinalUI()->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, _RotationY);
}

void CGameMgr::SetHPUI(float _curHP, float _semimaxHP, float _maxHP, float _curBoost, float _maxBoost, float _healAmount)
{
	CGameObject* HPUI = CGameMgr::GetInst()->GetHPUI();
	HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, _curHP / _maxHP);
	HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, _curBoost / _maxBoost);

	if (_healAmount == 100.f)
		HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_2, 1.f);
	else if (_healAmount == 0.f)
		HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_2, 0.f);
	else
		HPUI->UIRender()->GetMaterial(0)->SetScalarParam(FLOAT_2, min(_curHP + _healAmount, _semimaxHP) / _maxHP);
}

int CGameMgr::Init()
{
	// ScriptMgr 등록
	CScriptMgr::GetInst()->RegisterScript(L"CCameraScript", []() { return new CameraController; });
	CScriptMgr::GetInst()->RegisterScript(L"CMissileScript", []() { return new MissileProjectile; });
	CScriptMgr::GetInst()->RegisterScript(L"CPlayerScript", []() { return new PlayerCharacter; });
	//CScriptMgr::GetInst()->RegisterScript(L"WeaponController", []() { return new ; });
	CScriptMgr::GetInst()->RegisterScript(L"CGunScript", []() { return new GunController; });
	CScriptMgr::GetInst()->RegisterScript(L"CThrowableScript", []() { return new ThrowableController; });
	CScriptMgr::GetInst()->RegisterScript(L"BombScript", []() { return new BombController; });
	CScriptMgr::GetInst()->RegisterScript(L"TestSound", []() { return new TestSound; });
	CScriptMgr::GetInst()->RegisterScript(L"EnemyVision", []() { return new EnemyVisionScript; });
	//CScriptMgr::GetInst()->RegisterScript(L"EnemyController", []() { return new ; });
	CScriptMgr::GetInst()->RegisterScript(L"TestCharacter", []() { return new TestCharacter; });
	CScriptMgr::GetInst()->RegisterScript(L"CParticleScript", []() { return new ParticleController; });
	CScriptMgr::GetInst()->RegisterScript(L"InventoryScript", []() { return new InventoryController; });
	CScriptMgr::GetInst()->RegisterScript(L"ItemScript", []() { return new ItemScript; });
	CScriptMgr::GetInst()->RegisterScript(L"ItemUIScript", []() { return new ItemUI; });
	CScriptMgr::GetInst()->RegisterScript(L"InventoryUIScript", []() { return new InventoryUI; });
	CScriptMgr::GetInst()->RegisterScript(L"VicinityUIScript", []() { return new VicinityUI; });
	CScriptMgr::GetInst()->RegisterScript(L"InteractionHandler", []() { return new InteractionHandler; });
	CScriptMgr::GetInst()->RegisterScript(L"PauseUIScript", []() { return new PauseUIScript; });
	CScriptMgr::GetInst()->RegisterScript(L"DoorScript", []() { return new DoorScript; });
	//CScriptMgr::GetInst()->RegisterScript(L"InteractableScript", []() { return new; });
	CScriptMgr::GetInst()->RegisterScript(L"RoundsUIScript", []() { return new RoundsUIScript; });
	CScriptMgr::GetInst()->RegisterScript(L"KillinfoUIScript", []() { return new KillinfoUIScript; });
	CScriptMgr::GetInst()->RegisterScript(L"MinimapCameraScript", []() { return new MinimapCameraScript; });
	CScriptMgr::GetInst()->RegisterScript(L"MinimapUIScript", []() { return new MinimapUIScript; });
	CScriptMgr::GetInst()->RegisterScript(L"CameraEffect", []() { return new CameraEffect; });
	CScriptMgr::GetInst()->RegisterScript(L"TestFadeInOutReset", []() { return new TestFadeInOutReset; });
	CScriptMgr::GetInst()->RegisterScript(L"OptionUIScript", []() { return new OptionUIScript; });

	// StateMgr 등록
	CStateMgr::GetInst()->RegisterState(L"Player_Idle", []() { return new Player_Idle; });
	CStateMgr::GetInst()->RegisterState(L"Player_Jump", []() { return new Player_Jump; });
	CStateMgr::GetInst()->RegisterState(L"Player_Dead", []() { return new Player_Dead; });
	CStateMgr::GetInst()->RegisterState(L"Player_Heal", []() { return new Player_Heal; });
	CStateMgr::GetInst()->RegisterState(L"Player_Grenade_Throw_Low", []() { return new Player_Grenade_Throw_Low; });
	CStateMgr::GetInst()->RegisterState(L"Player_Grenade_Throw_High", []() { return new Player_Grenade_Throw_High; });
	CStateMgr::GetInst()->RegisterState(L"Player_Grenade_Prepare", []() { return new Player_Grenade_Prepare; });
	CStateMgr::GetInst()->RegisterState(L"Player_Gun_Reload", []() { return new Player_Gun_Reload; });
	CStateMgr::GetInst()->RegisterState(L"Player_Gun_Fire", []() { return new Player_Gun_Fire; });

	// LevelMgr InitCallback 등록
	CLevelMgr::GetInst()->RegisterLevelInitCallback([]() { CGameMgr::GetInst()->Begin(); });

	return S_OK;
}

void CGameMgr::Begin()
{
	// Main Camera
	m_MainCamera = CLevelMgr::GetInst()->FindObjectByName(L"MainCamera");

	// Player
	m_Player = CLevelMgr::GetInst()->FindObjectByName(L"Player");

	// Player UI
	m_InventoryCanvasUI = CLevelMgr::GetInst()->FindObjectByName(L"Inventory_CanvasUI");
	m_PauseCanvasUI = CLevelMgr::GetInst()->FindObjectByName(L"Pause_CanvasUI");
	m_OptionCanvasUI = CLevelMgr::GetInst()->FindObjectByName(L"Option_CanvasUI");
	m_CardinalImageUI = CLevelMgr::GetInst()->FindObjectByName(L"Cardinal_ImageUI");
	m_HPUI = CLevelMgr::GetInst()->FindObjectByName(L"HP_UI");
	m_ItemUseUI = CLevelMgr::GetInst()->FindObjectByName(L"ItemUse_UI");
	m_ReloadUI = CLevelMgr::GetInst()->FindObjectByName(L"Reload_UI");

	// Script (Camera, UI)
	m_CamScript = static_cast<CameraController*>(GetScriptWithType(m_MainCamera, SCRIPT_TYPE::CAMERASCRIPT));
	CGameObject* killinfoUI = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Killinfo_UI");
	m_KillinfoScript = static_cast<KillinfoUIScript*>(GetScriptWithType(killinfoUI, SCRIPT_TYPE::KILLINFOUI));
	CGameObject* CameraPost = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"CameraPost");
	m_CameraEffect = static_cast<CameraEffect*>(GetScriptWithType(CameraPost, SCRIPT_TYPE::CAMERAEFFECT));

	// Player Script
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));

	// Event
	m_FadeInOutEvent = static_cast<TestFadeInOutReset*>(GetScriptWithType(CLevelMgr::GetInst()->FindObjectByName(L"FadeInOut_Event"), SCRIPT_TYPE::TESTFADEINOUTRESET));
}

void CGameMgr::ResumeGame()
{
	// UI bool 값, Mouse 비활성화
	m_PlayerScript->SetPasueUIOff();

	// 인벤토리가 열려있다면 Mouse는 비활성화 하지 않는다.
	if (m_PlayerScript->IsInventoryOpened())
	{
		m_PlayerScript->SetMouseActive(true);
	}
	else
	{
		m_PlayerScript->SetMouseActive(false);
	}
		
	//  상태를 초기화해준다.
	m_CamScript->SetFlag(ADS, false);

	// UI 비 활성화
	SetObjectActive(CGameMgr::GetInst()->GetPauseCanvasUI(), false);
}


// ==========
// PauseMenu
// ==========
void CGameMgr::RestartGame()
{
	// Restart Event
	m_FadeInOutEvent->SetEventStart();

	// PauseUI 비활성화
	SetObjectActive(CGameMgr::GetInst()->GetPauseCanvasUI(), false);
	// Mouse 커서 비활성화
	m_PlayerScript->SetMouseActive(false);
}

void CGameMgr::OpenOption()
{
	// PauseUI 비활성화
	SetObjectActive(CGameMgr::GetInst()->GetPauseCanvasUI(), false);
	// OptionUI 활성화
	SetObjectActive(CGameMgr::GetInst()->GetOptionCanvasUI(), true);
	m_PlayerScript->SetOptionUIOpened(true);
}

void CGameMgr::ExitGame()
{
	// 프로그램 종료
	PostQuitMessage(0);
}


// ==========
// OptionMenu
// ==========
void CGameMgr::UpSensi()
{
	m_PlayerScript->PlusMouseSensitivity();
}

void CGameMgr::DownSensi()
{
	m_PlayerScript->MinusMouseSensitivity();
}

void CGameMgr::ExitOption()
{
	SetObjectActive(CGameMgr::GetInst()->GetPauseCanvasUI(), true);
	SetObjectActive(CGameMgr::GetInst()->GetOptionCanvasUI(), false);
	m_PlayerScript->SetOptionUIOpened(false);
}
