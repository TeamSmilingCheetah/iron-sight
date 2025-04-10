#include "pch.h"

#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CSoundMgr.h"

#include "Game/Gameplay/Weapon/Public/WeaponController.h"
#include "Game/Gameplay/Character/Public/CameraController.h"
#include "Game/Gameplay/Projectile/Public/MissileProjectile.h"


WeaponController::WeaponController(SCRIPT_TYPE _Type)
	: CScript(static_cast<UINT>(_Type))
	, m_EquippedOwner(nullptr)
{

}

WeaponController::~WeaponController()
{
}

void WeaponController::SaveComponent(FILE* _File)
{
}

void WeaponController::LoadComponent(FILE* _File)
{
}


