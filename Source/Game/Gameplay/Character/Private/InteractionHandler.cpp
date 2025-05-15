#include "pch.h"
#include "Game/Gameplay/Character/Public/InteractionHandler.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Inventory/Public/InventoryController.h"
#include "Game/Gameplay/Interaction/Public/InteractableScript.h"

#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Actor/CLayer.h"

InteractionHandler::InteractionHandler()
	: CScript(INTERACTION_HANDLER)
{
}

InteractionHandler::~InteractionHandler()
{
}

void InteractionHandler::SetPlayer(CGameObject* _Player)
{
	m_Player = _Player;
	m_PlayerScript = static_cast<PlayerCharacter*>(m_Player->GetScript(PLAYERSCRIPT));
}

void InteractionHandler::Init()
{
}

void InteractionHandler::Begin()
{
}

void InteractionHandler::Tick()
{
}

void InteractionHandler::SaveComponent(FILE* _File)
{
	SaveObjectRef(m_Player, _File);
}

void InteractionHandler::LoadComponent(FILE* _File)
{
	LoadObjectRef(m_Player, _File);
	// TODO : ObjectRef 수정.. ㅠㅠ
}

void InteractionHandler::BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	// Interactable Script
	// TODO : Script 개선
	auto pInteractable = static_cast<InteractableScript*>(_OtherObject->GetScript(ITEMSCRIPT));
	if (!pInteractable)
		pInteractable = static_cast<InteractableScript*>(_OtherObject->GetScript(DOORSCRIPT));

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
	// TODO : Script 개선
	auto pInteractable = static_cast<InteractableScript*>(_OtherObject->GetScript(ITEMSCRIPT));
	if (!pInteractable)
		pInteractable = static_cast<InteractableScript*>(_OtherObject->GetScript(DOORSCRIPT));

	if (!pInteractable)
		return;

	// interaction 발생시킴
	int layerIdx = pTarget->GetLayerIdx();

	bool isInteractableLayer = (layerIdx == 1 || layerIdx == 6);
	bool isInteractableChanged = false;

	if (isInteractableLayer)
	{
		// 인터랙션 가능한 오브젝트로 바뀐 경우
		if (!m_Interactable)
		{
			m_Interactable = true;
			isInteractableChanged = true;
		}

		// 오브젝트가 바뀌었으면 UI 설정
		if (m_InteractableObject != _OtherObject)
		{
			m_InteractableObject = _OtherObject;
			// TODO: Interaction UI 설정
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
			m_Interactable = false;
			isInteractableChanged = true;
		}
	}

	// Interaction UI 띄우기
	//if (isInteractableChanged)
	//	SetObjectActive(m_InteractionUI, m_Interactable);
}

void InteractionHandler::EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
{
	// Interactable Script
	// TODO : Script 개선
	auto pInteractable = static_cast<InteractableScript*>(_OtherObject->GetScript(ITEMSCRIPT));
	if (!pInteractable)
		pInteractable = static_cast<InteractableScript*>(_OtherObject->GetScript(DOORSCRIPT));

	if (!pInteractable)
		return;

	pInteractable->ExitDetection(this);
}
