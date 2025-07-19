#include "pch.h"
#include "Game/Gameplay/UI/Public/MinimapCameraScript.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"

MinimapCameraScript::MinimapCameraScript()
	: CScript(SCRIPT_TYPE::MINIMAPCAMERA)
	, m_Player(nullptr)
	, m_PlayerScript(nullptr)
{
}

MinimapCameraScript::~MinimapCameraScript()
{
}

void MinimapCameraScript::Begin()
{
	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));
}

void MinimapCameraScript::Tick()
{
	// 1. 카메라의 방향을 아래로 고정
	// 2. 플레이어 추적하여 화면을 플레이어 중앙으로 이동

	// 플레이어가 없으면 작동되어선 안됨
	if (!m_Player)
		return;

	// 1. 카메라의 방향을 아래로 고정
	Transform()->SetRelativeRotation(Vec3(90.f, 0.f, 0.f));

	// 2. 플레이어 추적하여 카메라를 플레이어 중앙으로 이동
	Vec3 playerPos = m_Player->Transform()->GetRelativePos();
	Vec3 cameraPos = Vec3(playerPos.x, 7000.f, playerPos.z);  // Y축은 높은 곳에 고정
	Transform()->SetRelativePos(cameraPos);

}

void MinimapCameraScript::SaveComponent(FILE* _File)
{
}

void MinimapCameraScript::LoadComponent(FILE* _File)
{
}

