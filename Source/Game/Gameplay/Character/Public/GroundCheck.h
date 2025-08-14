#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class GroundCheck
	: public CScript
{
private:
	class PlayerCharacter* m_PlayerScript;

public:
	virtual void BeginOverlap(IColliderBase* InSelf, IColliderBase* InOther) override;
	virtual void Overlap(IColliderBase* InSelf, IColliderBase* InOther) override;
	virtual void EndOverlap(IColliderBase* InSelf, IColliderBase* InOther) override;

	virtual void Begin() override;
	virtual void Tick() override;

	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

public:
	CLONE(GroundCheck)
	GroundCheck();
	~GroundCheck();
};
