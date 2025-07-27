#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class PlayerCharacter;
class InventoryController;

class InteractionHandler
	: public CScript
{
private:
	CGameObject*			m_Player;
	PlayerCharacter*		m_PlayerScript;

	bool					m_Interactable;
	CGameObject*			m_InteractableObject;
	CGameObject*			m_InteractionUI;

public:
	void SetPlayer(CGameObject* _Player);
	void SetInteractionUI(CGameObject* _UI) { m_InteractionUI = _UI; }

	void SetInteractable(bool _b);

public:
	virtual void Init() override;
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;
	virtual void LoadComponentReference();

	void BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;
	void Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;
	void EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;


public:
	CLONE(InteractionHandler);
	InteractionHandler();
	~InteractionHandler();
};
