#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class InventoryController;

class VicinityUI
	: public CScript
{
private:
	InventoryController* m_Controller;

public:
	void SetController(InventoryController* _Controller) { m_Controller = _Controller; }

public:
	void Tick() override {}
	virtual void SaveComponent(FILE* _File) {}
	virtual void LoadComponent(FILE* _File) {}

	// UI Event
	virtual void OnMouseDrop(const PayLoad& _payload) override;

public:
	CLONE_DISABLE(VicinityUI);
	VicinityUI();
	~VicinityUI();
};
