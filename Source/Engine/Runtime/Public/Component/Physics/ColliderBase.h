#pragma once
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Base/CComponent.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"

/**
 * @brief 모든 Collider의 기본 클래스
 *
 * @param OverlapCount 현재 Overlap 상태인 다른 충돌체의 수
 * @param State 충돌체의 활성화 상태
 */
class IColliderBase : public CComponent
{
private:
	int OverlapCount;
	COLLIDER_STATE State;

protected:
	// Only Derived Class Can Use Constructor Function
	IColliderBase(COMPONENT_TYPE InType);

public:
	void FinalTick() override = 0;
	void SaveComponent(FILE* InFile) override = 0;
	void LoadComponent(FILE* InFile) override = 0;
	virtual const AABB GetAABB() const = 0;

	// Getter & Setter
	COLLIDER_STATE GetState() const { return State; }
	bool IsActive() const { return State == ACTIVE; }
	bool IsOverlapped() const { return OverlapCount != 0; }

	void SetActivate() { State = ACTIVE; }
	void SetDeactivate() { State = SEMIDEACTIVE; }

	// Templated Overlap Functions
	template <typename T>
	void BeginOverlap(T* InOther);
	template <typename T>
	void Overlap(T* InOther);
	template <typename T>
	void EndOverlap(T* InOther);

	// Special Member Function
	~IColliderBase() override;
	IColliderBase(const IColliderBase& POrigin);
};

inline IColliderBase::IColliderBase(COMPONENT_TYPE InType)
	: CComponent(InType), OverlapCount(0), State(ACTIVE)
{
}

inline IColliderBase::IColliderBase(const IColliderBase& POrigin)
	: CComponent(POrigin)
	  , OverlapCount(0)
	  , State(POrigin.State)
{
}

inline IColliderBase::~IColliderBase() = default;

template <typename T>
void IColliderBase::BeginOverlap(T* InOther)
{
	++OverlapCount;

	const vector<CScript*>& ScriptVector = GetOwner()->GetScripts();
	for (auto* Script : ScriptVector)
	{
		Script->BeginOverlap(this, InOther->GetOwner(), InOther);
	}
}

template <typename T>
void IColliderBase::Overlap(T* InOther)
{
	const vector<CScript*>& ScriptVector = GetOwner()->GetScripts();
	for (auto* Script : ScriptVector)
	{
		Script->Overlap(this, InOther->GetOwner(), InOther);
	}
}

template <typename T>
void IColliderBase::EndOverlap(T* InOther)
{
	--OverlapCount;

	const vector<CScript*>& ScriptVector = GetOwner()->GetScripts();
	for (auto* Script : ScriptVector)
	{
		Script->EndOverlap(this, InOther->GetOwner(), InOther);
	}
}
