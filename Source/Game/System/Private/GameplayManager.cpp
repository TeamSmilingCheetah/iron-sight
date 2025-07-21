#include "pch.h"
#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Projectile/Public/MissileProjectile.h"
#include "Game/Gameplay/Weapon/Public/GunController.h"
#include "Game/Gameplay/Weapon/Public/ThrowableController.h"
#include "Game/Gameplay/UI/Public/RoundsUIScript.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Inventory/Public/Item.h"
#include "Game/Gameplay/Inventory/Public/UI_Item.h"
#include "Game/Gameplay/Inventory/Public/UI_Vicinity.h"
#include "Game/Gameplay/Inventory/Public/UI_Inventory.h"
#include "Game/Gameplay/TestSound.h"
#include "Game/Gameplay/Weapon/Public/BombController.h"
#include "Game/Gameplay/Character/Public/TestCharacter.h"
#include "Game/Gameplay/Character/Public/EnemyVisionScript.h"
#include "Game/Gameplay/Character/Public/InteractionHandler.h"
#include "Game/Gameplay/Door/Public/DoorScript.h"
#include "Game/Gameplay/Particle/Public/ParticleController.h"
#include "Game/Gameplay/UI/Public/KillinfoUIScript.h"
#include "Game/Gameplay/UI/Public/MinimapCameraScript.h"
#include "Game/Gameplay/UI/Public/MinimapUIScript.h"
#include "Game/Gameplay/Character/Public/CameraEffect.h"

void GameplayManager::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CCameraScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CGunScript");
	_vec.push_back(L"CThrowableScript");
	_vec.push_back(L"CParticleScript");
	_vec.push_back(L"TestSound");
	_vec.push_back(L"InventoryScript");
	_vec.push_back(L"ItemScript");
	_vec.push_back(L"ItemUIScript");
	_vec.push_back(L"InventoryUIScript");
	_vec.push_back(L"VicinityUIScript");
	_vec.push_back(L"EnemyController");
	_vec.push_back(L"EnemyVision");
	_vec.push_back(L"InteractionHandler");
	_vec.push_back(L"DoorScript");
	_vec.push_back(L"BombScript");
	_vec.push_back(L"RoundsUIScript");
	_vec.push_back(L"KillinfoUIScript");
	_vec.push_back(L"MinimapCameraScript");
	_vec.push_back(L"MinimapUIScript");
	_vec.push_back(L"CameraEffect");
}

CScript* GameplayManager::GetScript(const wstring& _strScriptName)
{
	if (L"CCameraScript" == _strScriptName)
		return new CameraController;
	if (L"CMissileScript" == _strScriptName)
		return new MissileProjectile;
	if (L"CPlayerScript" == _strScriptName)
		return new PlayerCharacter;
	if (L"CGunScript" == _strScriptName)
		return new GunController;
	if (L"CThrowableScript" == _strScriptName)
		return new ThrowableController;
	if (L"TestSound" == _strScriptName)
		return new TestSound;
	if (L"CParticleScript" == _strScriptName)
		return new ParticleController;
	if (L"InventoryScript" == _strScriptName)
		return new InventoryController;
	if (L"ItemScript" == _strScriptName)
		return new ItemScript;
	if (L"ItemUIScript" == _strScriptName)
		return new ItemUI;
	if (L"InventoryUIScript" == _strScriptName)
		return new InventoryUI;
	if (L"VicinityUIScript" == _strScriptName)
		return new VicinityUI;
	if (L"TestCharacter" == _strScriptName)
		return new TestCharacter;
	if (L"EnemyVision" == _strScriptName)
		return new EnemyVisionScript;
	if (L"InteractionHandler" == _strScriptName)
		return new InteractionHandler;
	if (L"DoorScript" == _strScriptName)
		return new DoorScript;
	if (L"BombScript" == _strScriptName)
		return new BombController;
	if (L"RoundsUIScript" == _strScriptName)
		return new RoundsUIScript;
	if (L"KillinfoUIScript" == _strScriptName)
		return new KillinfoUIScript;
	if (L"MinimapCameraScript" == _strScriptName)
		return new MinimapCameraScript;
	if (L"MinimapUIScript" == _strScriptName)
		return new MinimapUIScript;
	if (L"CameraEffect" == _strScriptName)
		return new CameraEffect;

	return nullptr;
}

CScript* GameplayManager::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case static_cast<UINT>(SCRIPT_TYPE::CAMERASCRIPT):
		return new CameraController;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::MISSILESCRIPT):
		return new MissileProjectile;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT):
		return new PlayerCharacter;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::WEAPONSCRIPT):
		return new GunController;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::THROWABLESCRIPT):
		return new ThrowableController;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::TESTSOUND):
		return new TestSound;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::PARTICLESCRIPT):
		return new ParticleController;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::INVENTORYSCRIPT):
		return new InventoryController;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::ITEMSCRIPT):
		return new ItemScript;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::ITEMUI):
		return new ItemUI;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::INVENTORYUI):
		return new InventoryUI;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::VICINITYUI):
		return new VicinityUI;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::TESTCHARACTER):
		return new TestCharacter;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::ENEMYVISION):
		return new EnemyVisionScript;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::INTERACTION_HANDLER):
		return new InteractionHandler;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::DOORSCRIPT):
		return new DoorScript;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::BOMBSCRIPT):
		return new BombController;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::ROUNDSSUI):
		return new RoundsUIScript;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::KILLINFOUI):
		return new KillinfoUIScript;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::MINIMAPCAMERA):
		return new MinimapCameraScript;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::MINIMAPUI):
		return new MinimapUIScript;
		break;
	case static_cast<UINT>(SCRIPT_TYPE::CAMERAEFFECT):
		return new CameraEffect;
		break;
	}
	return nullptr;
}

const wchar_t* GameplayManager::GetScriptName(CScript* _pScript)
{
	switch (_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::CAMERASCRIPT:
		return L"CCameraScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::GUNSCRIPT:
		return L"CGunScript";
		break;

	case SCRIPT_TYPE::THROWABLESCRIPT:
		return L"CThrowableScript";
		break;

	case SCRIPT_TYPE::TESTSOUND:
		return L"TestSound";
		break;

	case SCRIPT_TYPE::PARTICLESCRIPT:
		return L"CParticleScript";
		break;

	case SCRIPT_TYPE::INVENTORYSCRIPT:
		return L"InventoryScript";
		break;

	case SCRIPT_TYPE::ITEMSCRIPT:
		return L"ItemScript";
		break;

	case SCRIPT_TYPE::ITEMUI:
		return L"ItemUIScript";
		break;

	case SCRIPT_TYPE::INVENTORYUI:
		return L"InventoryUIScript";
		break;

	case SCRIPT_TYPE::VICINITYUI:
		return L"VicinityUIScript";

	case SCRIPT_TYPE::TESTCHARACTER:
		return L"TestCharacter";
		break;

	case SCRIPT_TYPE::ENEMYVISION:
		return L"EnemyVision";
		break;

	case SCRIPT_TYPE::INTERACTION_HANDLER:
		return L"InteractionHandler";
		break;

	case SCRIPT_TYPE::DOORSCRIPT:
		return L"DoorScript";
		break;

	case SCRIPT_TYPE::BOMBSCRIPT:
		return L"BombScript";
		break;

	case SCRIPT_TYPE::ROUNDSSUI:
		return L"RoundsUIScript";
		break;

	case SCRIPT_TYPE::KILLINFOUI:
		return L"KillinfoUIScript";
		break;

	case SCRIPT_TYPE::MINIMAPCAMERA:
		return L"MinimapCameraScript";
		break;

	case SCRIPT_TYPE::MINIMAPUI:
		return L"MinimapUIScript";
		break;

	case SCRIPT_TYPE::CAMERAEFFECT:
		return L"CameraEffect";
		break;
	}
	return nullptr;
}
