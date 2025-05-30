#include "pch.h"
#include "Game/Gameplay/Character/Public/InteractionHandler.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Interaction/Public/InteractableScript.h"

#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Actor/CLayer.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"

InteractionHandler::InteractionHandler()
	: CScript(SCRIPT_TYPE::INTERACTION_HANDLER)
{
}

InteractionHandler::~InteractionHandler()
{
}

void InteractionHandler::SetPlayer(CGameObject* _Player)
{
	m_Player = _Player;
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));
}

void InteractionHandler::SetInteractable(bool _b)
{
	m_Interactable = _b;
	SetObjectActive(m_InteractionUI, m_Interactable);

	if (!m_Interactable)
		m_InteractableObject = nullptr;
}

void InteractionHandler::Init()
{
}

void InteractionHandler::Begin()
{
}

void InteractionHandler::Tick()
{
	// 플레이어가 바라보고 있는 오브젝트
	CGameObject* pTarget = m_PlayerScript->GetRayTarget();

	// null이라면 Interaction 불가능
	if (!pTarget || pTarget != m_InteractableObject)
	{
		SetInteractable(false);
	}
}

void InteractionHandler::SaveComponent(FILE* _File)
{
	SaveObjectRef(m_Player, _File);
	SaveObjectRef(m_InteractionUI, _File);
}

void InteractionHandler::LoadComponent(FILE* _File)
{
	LoadObjectRef(m_Player, _File);
	LoadObjectRef(m_InteractionUI, _File);
}

void InteractionHandler::LoadComponentReference()
{
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));
}

void InteractionHandler::BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	// Interactable Script
	auto pInteractable = static_cast<InteractableScript*>(GetScriptWithType(_OtherObject, SCRIPT_TYPE::INTERACTABLE));
	
	if (!pInteractable)
		return;

	pInteractable->EnterDetection(this);
}

void InteractionHandler::Overlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	// 플레이어가 바라보고 있는 오브젝트
	CGameObject* pTarget = m_PlayerScript->GetRayTarget();

	// 바라보고 있는 오브젝트가 존재하지 않거나 충돌 오브젝트와 다르다면 리턴
	if (!pTarget || pTarget != _OtherObject)
		return;

	// Interactable Script
	auto pInteractable = static_cast<InteractableScript*>(GetScriptWithType(_OtherObject, SCRIPT_TYPE::INTERACTABLE));

	if (pInteractable)
	{
		// 인터랙션 가능한 오브젝트로 바뀐 경우
		if (!m_Interactable)
		{
			SetInteractable(true);
		}

		// 오브젝트가 바뀌었으면 UI 설정
		if (m_InteractableObject != _OtherObject)
		{
			m_InteractableObject = _OtherObject;
			// TODO: Interaction UI 설정
			m_InteractionUI->UI()->GetTextInfoRef()[0].Text = pInteractable->GetInteractionDesc();
		}

		// F 키 입력에 따라 상호작용
		if (KEY_TAP(KEY::F))
		{
			pInteractable->Interact(this);
		}
	}
	else
	{
		// 상호작용 불가 상태로 변경
		if (m_Interactable)
		{
			SetInteractable(false);
		}
	}
}

void InteractionHandler::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	// Interactable Script
	auto pInteractable = static_cast<InteractableScript*>(GetScriptWithType(_OtherObject, SCRIPT_TYPE::INTERACTABLE));
	if (!pInteractable)
		return;

	if (m_InteractableObject == _OtherObject)
	{
		SetInteractable(false);
	}

	pInteractable->ExitDetection(this);
}
