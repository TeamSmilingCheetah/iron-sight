#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"


enum class MOTION_STATE : uint8_t
{
	STAND,	// 서 있음
	CROUCH,	// 앉아 있음
	PRONE,	// 엎드려 있음
};

enum class ACTION_STATE : uint8_t
{
	JUMP,

	GUN_FIRE,
	GUN_RELOAD,

	GRENADE_PREPARE,
	GRENADE_THROW_LOW,
	GRENADE_THROW_HIGH,

	BANDAGE,
	MED_KIT,
	FIRST_AID_KIT,

	ENERGY_DRINK,
	PAIN_KILLER,
	ADRENALINE_SYRINGE,

	DEAD,

	NONE,
};


class CState;

class CStateMachine :
	public CComponent
{
private:
	map<UINT, CState*> m_mapActionState;

	MOTION_STATE m_CurMotionState;
	CState*		 m_CurActionState;

	MOTION_STATE m_PrevMotionState;
	CState*		 m_PrevActionState;

public:
	MOTION_STATE GetCurMotionState() { return m_CurMotionState; }
	CState* GetCurActionState() { return m_CurActionState; }

	MOTION_STATE GetPrevMotionState() { return m_PrevMotionState; }
	CState* GetPrevActionState() { return m_PrevActionState; }

	void ChangeActionState(ACTION_STATE _State);
	void SetMoitionState(MOTION_STATE _State) { m_PrevMotionState = m_CurMotionState; m_CurMotionState = _State; }


	void AddState(CState* _State);
	CState* FindState(ACTION_STATE _State);
public:
	virtual void FinalTick() override;
	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

public:
	CLONE(CStateMachine);
	CStateMachine();
	CStateMachine(const CStateMachine& _Other);
	~CStateMachine() override;
};

