#pragma once
#include "Client/UI/Public/Component/ComponentUI.h"

#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"

class StateMachineUI :
	public ComponentUI
{
private:

public:
	void Render_Update() override;

private:

	string GetActionStateStr(ACTION_STATE _State);
	string GetMotionStateStr(MOTION_STATE _State);

public:
	StateMachineUI();
	~StateMachineUI() override;
};

