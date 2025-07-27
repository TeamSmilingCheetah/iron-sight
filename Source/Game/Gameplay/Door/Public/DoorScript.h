#pragma once
#include "Game/Gameplay/Interaction/Public/InteractableScript.h"

class DoorScript
	: public InteractableScript
{
private:
	bool	m_Opened;
	float			m_CurClipAccTime;
	const float		m_Duration;

	float	m_OrigAngle;
	float	m_ChangeAngle;
	bool	m_Rotating;

public:
	virtual void Init() override;
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

	virtual void BeginOverlap(class FCollider3D* _Collider, CGameObject* _OtherObject, FCollider3D* _OtherCollider) override;
	virtual void Overlap(class FCollider3D* _Collider, CGameObject* _OtherObject, FCollider3D* _OtherCollider) override;
	virtual void EndOverlap(class FCollider3D* _Collider, CGameObject* _OtherObject, FCollider3D* _OtherCollider) override;

	virtual void EnterDetection(InteractionHandler* _Handler) override {}
	virtual void Interact(InteractionHandler* _Handler) override;
	virtual void ExitDetection(InteractionHandler* _Handler) override {}

public:
	SET_PARENT_SCRIPT(SCRIPT_TYPE::INTERACTABLE);
	CLONE(DoorScript);
	DoorScript();
	~DoorScript();
};
