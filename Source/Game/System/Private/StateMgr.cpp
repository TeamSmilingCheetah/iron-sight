#include "pch.h"


#include "Game/System/Public/StateMgr.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

#include "Game/System/Public/states.h"


void StateMgr::GetStateInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"IdleState");
	_vec.push_back(L"JumpState");
	_vec.push_back(L"DeadState");
	_vec.push_back(L"PainKillerState");
	_vec.push_back(L"MedKitState");
	_vec.push_back(L"FirstAidKitState");
	_vec.push_back(L"EnergyDrinkState");
	_vec.push_back(L"BandageState");
	_vec.push_back(L"AdrenalineState");
	_vec.push_back(L"GrenadeThrowLowState");
	_vec.push_back(L"GrenadeThrowHighState");
	_vec.push_back(L"GrenadePrepareState");
	_vec.push_back(L"GunReloadState");
	_vec.push_back(L"GunReloadState");
}

CState* StateMgr::GetState(const wstring& _strStateName)
{

	if (L"IdleState" == _strStateName)
		return new IdleState;

	if (L"JumpState" == _strStateName)
		return new JumpState;

	if (L"DeadState" == _strStateName)
		return new DeadState;

	if (L"PainKillerState" == _strStateName)
		return new PainKillerState;

	if (L"FirstAidKitState" == _strStateName)
		return new FirstAidKitState;

	if (L"BandageState" == _strStateName)
		return new BandageState;

	if (L"AdrenalineState" == _strStateName)
		return new AdrenalineState;

	if (L"GrenadeThrowLowState" == _strStateName)
		return new GrenadeThrowLowState;

	if (L"GrenadeThrowHighState" == _strStateName)
		return new GrenadeThrowHighState;

	if (L"GrenadePrepareState" == _strStateName)
		return new GrenadePrepareState;

	if (L"GunReloadState" == _strStateName)
		return new GunReloadState;

	if (L"GunFireState" == _strStateName)
		return new GunFireState;

	return nullptr;
}

CState* StateMgr::GetState(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case static_cast<UINT>(ACTION_STATE::NONE):
		return new IdleState;
		break;

	case static_cast<UINT>(ACTION_STATE::JUMP):
		return new JumpState;
		break;

	case static_cast<UINT>(ACTION_STATE::DEAD):
		return new DeadState;
		break;

	case static_cast<UINT>(ACTION_STATE::PAIN_KILLER):
		return new PainKillerState;
		break;

	case static_cast<UINT>(ACTION_STATE::MED_KIT):
		return new MedKitState;
		break;

	case static_cast<UINT>(ACTION_STATE::FIRST_AID_KIT):
		return new FirstAidKitState;
		break;

	case static_cast<UINT>(ACTION_STATE::ENERGY_DRINK):
		return new EnergyDrinkState;
		break;

	case static_cast<UINT>(ACTION_STATE::BANDAGE):
		return new BandageState;
		break;

	case static_cast<UINT>(ACTION_STATE::ADRENALINE_SYRINGE):
		return new AdrenalineState;
		break;

	case static_cast<UINT>(ACTION_STATE::GRENADE_THROW_LOW):
		return new GrenadeThrowLowState;
		break;

	case static_cast<UINT>(ACTION_STATE::GRENADE_THROW_HIGH):
		return new GrenadeThrowHighState;
		break;

	case static_cast<UINT>(ACTION_STATE::GRENADE_PREPARE):
		return new GrenadePrepareState;
		break;

	case static_cast<UINT>(ACTION_STATE::GUN_RELOAD):
		return new GunReloadState;
		break;

	case static_cast<UINT>(ACTION_STATE::GUN_FIRE):
		return new GunFireState;
		break;

	}
	return nullptr;
}

const wchar_t* StateMgr::GetStateName(CState* _pState)
{
	switch (_pState->GetOwnState())
	{
	case ACTION_STATE::NONE:
		return L"IdleState";
		break;

	case ACTION_STATE::JUMP:
		return L"JumpState";
		break;

	case ACTION_STATE::DEAD:
		return L"DeadState";
		break;

	case ACTION_STATE::PAIN_KILLER:
		return L"PainKillerState";
		break;

	case ACTION_STATE::MED_KIT:
		return L"MedKitState";
		break;

	case ACTION_STATE::FIRST_AID_KIT:
		return L"FirstAidKitState";
		break;

	case ACTION_STATE::ENERGY_DRINK:
		return L"EnergyDrinkState";
		break;

	case ACTION_STATE::BANDAGE:
		return L"BandageState";
		break;

	case ACTION_STATE::ADRENALINE_SYRINGE:
		return L"AdrenalineState";
		break;

	case ACTION_STATE::GRENADE_THROW_LOW:
		return L"GrenadeThrowLowState";
		break;

	case ACTION_STATE::GRENADE_THROW_HIGH:
		return L"GrenadeThrowHighState";
		break;

	case ACTION_STATE::GRENADE_PREPARE:
		return L"GrenadePrepareState";
		break;

	case ACTION_STATE::GUN_RELOAD:
		return L"GunReloadState";
		break;

	case ACTION_STATE::GUN_FIRE:
		return L"GunFireState";
		break;

	}
	return nullptr;
}
