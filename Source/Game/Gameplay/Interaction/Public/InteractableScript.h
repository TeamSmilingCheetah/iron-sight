#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class InteractionHandler;

class InteractableScript
	: public CScript
{
protected:
	wstring		m_InteractionDesc;

public:
	const wstring& GetInteractionDesc() const { return m_InteractionDesc; }

	virtual void EnterDetection(InteractionHandler* _Handler) = 0;
	virtual void Interact(InteractionHandler* _Handler) = 0;
	virtual void ExitDetection(InteractionHandler* _Handler) = 0;

public:
	InteractableScript(SCRIPT_TYPE _Type);
	~InteractableScript();
};
