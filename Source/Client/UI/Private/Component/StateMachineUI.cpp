#include "pch.h"

#include "Client/UI/Public/Component/StateMachineUI.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/Runtime/Public/State/CState.h"

StateMachineUI::StateMachineUI()
	: ComponentUI("StateMachineUI", COMPONENT_TYPE::STATEMACINE)
{
}

StateMachineUI::~StateMachineUI()
{
}

void StateMachineUI::Render_Update()
{
	ComponentTitle("StateMachine");

	CStateMachine* pSM = GetTargetObject()->StateMachine();

	CState* pState = pSM->GetCurActionState();

	if (nullptr == pState)
	{
		return;
	}
	ACTION_STATE CurActionState = pState->GetOwnState();
	MOTION_STATE CurMotionState = pSM->GetCurMotionState();

	string strAction = GetActionStateStr(CurActionState);
	string strMotion = GetMotionStateStr(CurMotionState);

	ImGui::Text("CurAction");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150);
	ImGui::InputText("##ActionName", (char*)strAction.c_str(), strAction.length(),
		ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("CurMotion");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150);
	ImGui::InputText("##MotionName", (char*)strMotion.c_str(), strMotion.length(),
		ImGuiInputTextFlags_ReadOnly);
}

string StateMachineUI::GetActionStateStr(ACTION_STATE _State)
{
	switch (_State)
	{
	case ACTION_STATE::JUMP:
		return "Jump";
		break;
	case ACTION_STATE::GUN_FIRE:
		return "Gun_Fire";
		break;
	case ACTION_STATE::GUN_RELOAD:
		return "Gun_Reload";
		break;
	case ACTION_STATE::GRENADE_PREPARE:
		return "Grenade_Prepare";
		break;
	case ACTION_STATE::GRENADE_THROW_LOW:
		return "Grenade_Throw_Low";
		break;
	case ACTION_STATE::GRENADE_THROW_HIGH:
		return "Grenade_Throw_High";
		break;
	case ACTION_STATE::BANDAGE:
		return "Bandage";
		break;
	case ACTION_STATE::MED_KIT:
		return "Med_Kit";
		break;
	case ACTION_STATE::FIRST_AID_KIT:
		return "First_Aid_Kit";
		break;
	case ACTION_STATE::ENERGY_DRINK:
		return "Energy_Drink";
		break;
	case ACTION_STATE::PAIN_KILLER:
		return "Pain_Killer";
		break;
	case ACTION_STATE::ADRENALINE_SYRINGE:
		return "Adrenaline_Syringe";
		break;
	case ACTION_STATE::DEAD:
		return "Dead";
		break;
	case ACTION_STATE::NONE:
		return "Idle";
		break;
	}

	return "NO ACTION";
}

string StateMachineUI::GetMotionStateStr(MOTION_STATE _State)
{
	switch (_State)
	{
	case MOTION_STATE::STAND:
		return "Stand";
		break;
	case MOTION_STATE::CROUCH:
		return "Crouch";
		break;
	case MOTION_STATE::PRONE:
		return "Prone";
		break;
	}

	return "NO MOTION";
}
