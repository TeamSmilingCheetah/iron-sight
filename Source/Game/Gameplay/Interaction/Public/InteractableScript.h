#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class InteractionHandler;

class InteractableScript
	: public CScript
{
public:
	virtual void EnterDetection(InteractionHandler* _Handler) = 0;
	virtual void Interact(InteractionHandler* _Handler) = 0;
	virtual void ExitDetection(InteractionHandler* _Handler) = 0;

public:
	InteractableScript(SCRIPT_TYPE _Type);
	~InteractableScript();
};
