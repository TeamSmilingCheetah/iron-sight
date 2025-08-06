#include "pch.h"
#include "Game/System/Public/CGameMgr.h"

#include "Engine/System/Public/Manager/CStateMgr.h"
#include "Engine/System/Public/Manager/CScriptMgr.h"

#include "Game/Gameplay/states.h"
#include "Game/Gameplay/scripts.h"

CGameMgr::CGameMgr()
{

}

CGameMgr::~CGameMgr()
{

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
	CScriptMgr::GetInst()->RegisterScript(L"DoorScript", []() { return new DoorScript; });
	//CScriptMgr::GetInst()->RegisterScript(L"InteractableScript", []() { return new; });
	CScriptMgr::GetInst()->RegisterScript(L"RoundsUIScript", []() { return new RoundsUIScript; });
	CScriptMgr::GetInst()->RegisterScript(L"KillinfoUIScript", []() { return new KillinfoUIScript; });
	CScriptMgr::GetInst()->RegisterScript(L"MinimapCameraScript", []() { return new MinimapCameraScript; });
	CScriptMgr::GetInst()->RegisterScript(L"MinimapUIScript", []() { return new MinimapUIScript; });
	CScriptMgr::GetInst()->RegisterScript(L"CameraEffect", []() { return new CameraEffect; });


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


	return S_OK;
}
