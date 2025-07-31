#include "pch.h"


#include "Game/System/Public/StateMgr.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

#include "Game/System/Public/states.h"


void StateMgr::GetStateInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"Player_Idle");
	_vec.push_back(L"Player_Jump");
	_vec.push_back(L"Player_Dead");
	_vec.push_back(L"Player_MedKit");
	_vec.push_back(L"Player_PainKiller");
	_vec.push_back(L"Player_FirstAidKit");
	_vec.push_back(L"Player_EnergyDrink");
	_vec.push_back(L"Player_Bandage");
	_vec.push_back(L"Player_Adrenaline");
	_vec.push_back(L"Player_Grenade_Throw_Low");
	_vec.push_back(L"Player_Grenade_Throw_High");
	_vec.push_back(L"Player_Grenade_Prepare");
	_vec.push_back(L"Player_Gun_Reload");
	_vec.push_back(L"Player_Gun_Fire");
}

CState* StateMgr::GetState(const wstring& _strStateName)
{
	if (L"Player_Idle" == _strStateName)
		return new Player_Idle;

	if (L"Player_Jump" == _strStateName)
		return new Player_Jump;

	if (L"Player_Dead" == _strStateName)
		return new Player_Dead;

	if (L"Player_MedKit" == _strStateName)
		return new Player_MedKit;

	if (L"Player_PainKiller" == _strStateName)
		return new Player_PainKiller;

	if (L"Player_FirstAidKit" == _strStateName)
		return new Player_FirstAidKit;

	if (L"Player_EnergyDrink" == _strStateName)
		return new Player_EnergyDrink;

	if (L"Player_Bandage" == _strStateName)
		return new Player_Bandage;

	if (L"Player_Adrenaline" == _strStateName)
		return new Player_Adrenaline;

	if (L"Player_Grenade_Throw_Low" == _strStateName)
		return new Player_Grenade_Throw_Low;

	if (L"Player_Grenade_Throw_High" == _strStateName)
		return new Player_Grenade_Throw_High;

	if (L"Player_Grenade_Prepare" == _strStateName)
		return new Player_Grenade_Prepare;

	if (L"Player_Gun_Reload" == _strStateName)
		return new Player_Gun_Reload;

	if (L"Player_Gun_Fire" == _strStateName)
		return new Player_Gun_Fire;

	return nullptr;
}


const wchar_t* StateMgr::GetStateName(CState* _pState)
{
	return _pState->GetName().c_str();
}
