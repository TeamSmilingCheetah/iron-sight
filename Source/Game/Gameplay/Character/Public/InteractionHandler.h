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

public:
	virtual void Init() override;
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) override;
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) override;
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) override;


public:
	CLONE(InteractionHandler);
	InteractionHandler();
	~InteractionHandler();
};
