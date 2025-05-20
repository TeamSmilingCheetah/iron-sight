#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class InventoryController;

class InventoryUI
	: public CScript
{
private:
	CGameObject*			m_ControllerOwner;
	InventoryController*	m_Controller;

public:
	void SetController(InventoryController* _Controller);

public:
	void Begin() override;
	void Tick() override {}
	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;
	virtual void LoadComponentReference() override;

	// UI Event
	virtual void OnMouseDrop(const PayLoad& _payload) override;

public:
	CLONE_DISABLE(InventoryUI);
	InventoryUI();
	~InventoryUI();
};
