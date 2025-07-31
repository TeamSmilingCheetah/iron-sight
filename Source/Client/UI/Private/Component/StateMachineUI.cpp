#include "pch.h"

#include "Client/UI/Public/Component/StateMachineUI.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"
#include "Engine/Runtime/Public/State/CState.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

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

	CState* pState = pSM->GetCurState();

	if (nullptr == pState)
	{
		return;
	}

	string strState = WStringToString(pState->GetName());

	ImGui::Text("CurState");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150);
	ImGui::InputText("##ActionName", (char*)strState.c_str(), strState.length(),
		ImGuiInputTextFlags_ReadOnly);
}
