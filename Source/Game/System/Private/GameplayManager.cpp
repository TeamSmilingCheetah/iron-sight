#include "pch.h"
#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Projectile/Public/MissileProjectile.h"

void GameplayManager::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CCameraScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CPlayerScript");
}

CScript* GameplayManager::GetScript(const wstring& _strScriptName)
{
	if (L"CCameraScript" == _strScriptName)
		return new CameraController;
	if (L"CMissileScript" == _strScriptName)
		return new MissileProjectile;
	if (L"CPlayerScript" == _strScriptName)
		return new PlayerCharacter;
	return nullptr;
}

CScript* GameplayManager::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case static_cast<UINT>(CAMERASCRIPT):
		return new CameraController;
		break;
	case static_cast<UINT>(MISSILESCRIPT):
		return new MissileProjectile;
		break;
	case static_cast<UINT>(PLAYERSCRIPT):
		return new PlayerCharacter;
		break;
	}
	return nullptr;
}

const wchar_t* GameplayManager::GetScriptName(CScript* _pScript)
{
	switch (static_cast<SCRIPT_TYPE>(_pScript->GetScriptType()))
	{
	case CAMERASCRIPT:
		return L"CCameraScript";
		break;

	case MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case PLAYERSCRIPT:
		return L"CPlayerScript";
		break;
	}
	return nullptr;
}
