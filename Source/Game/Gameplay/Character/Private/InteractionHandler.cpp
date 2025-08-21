#include "pch.h"
#include "Game/Gameplay/Character/Public/InteractionHandler.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Interaction/Public/InteractableScript.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"
#include "Engine/Runtime/Public/Component/Physics/ColliderBase.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"

InteractionHandler::InteractionHandler()
	: CScript(SCRIPT_TYPE::INTERACTION_HANDLER)
	  , m_Player(nullptr)
	  , m_PlayerScript(nullptr)
	  , m_Interactable(false)
	  , m_InteractableObject(nullptr)
	  , m_InteractionUI(nullptr)
{
}

InteractionHandler::~InteractionHandler() = default;

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

	// 재장전중이라면 취소 UI 띄어줌
	//if (m_PlayerScript->IsPlayerReloading())
	//{
	//	SetInteractable(true);
	//}

	//// 재장전이 끝나면 UI 종료
	//if (m_PlayerScript->IsPlayerReloadingEnd())
	//{
	//	SetInteractable(false);
	//	m_PlayerScript->SetReloadingEnd(false);
	//}
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

void InteractionHandler::BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::SphereCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		// Interactable Script
		auto pInteractable = static_cast<InteractableScript*>(
			GetScriptWithType(OtherObject, SCRIPT_TYPE::INTERACTABLE));

		if (!pInteractable)
			return;

		pInteractable->EnterDetection(this);
	}
}

void InteractionHandler::Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::SphereCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		// 플레이어가 바라보고 있는 오브젝트
		CGameObject* pTarget = m_PlayerScript->GetRayTarget();

		// 바라보고 있는 오브젝트가 존재하지 않거나 충돌 오브젝트와 다르다면 리턴
		if (!pTarget || pTarget != OtherObject)
			return;

		// Interactable Script
		auto pInteractable = static_cast<InteractableScript*>(
			GetScriptWithType(OtherObject, SCRIPT_TYPE::INTERACTABLE));

		if (pInteractable)
		{
			// 인터랙션 가능한 오브젝트로 바뀐 경우
			if (!m_Interactable)
			{
				SetInteractable(true);
			}

			// 오브젝트가 바뀌었으면 UI 설정
			if (m_InteractableObject != OtherObject)
			{
				m_InteractableObject = OtherObject;
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
}

void InteractionHandler::EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider)
{
	if (InCollider->GetColliderType() == EColliderType::SphereCollider &&
		InOtherCollider->GetColliderType() == EColliderType::BoxCollider)
	{
		FBoxCollider* OtherCollider = static_cast<FBoxCollider*>(InOtherCollider);
		CGameObject* OtherObject = OtherCollider->GetOwner();

		// Interactable Script
		auto pInteractable = static_cast<InteractableScript*>(
			GetScriptWithType(OtherObject, SCRIPT_TYPE::INTERACTABLE));
		if (!pInteractable)
			return;

		if (m_InteractableObject == OtherObject)
		{
			SetInteractable(false);
		}

		pInteractable->ExitDetection(this);
	}
}
