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


	map<wstring, CState*> pMap = pSM->GetStateMap();

	int stateCounts = static_cast<int>(pMap.size());
	ImGui::Text("Cur States Counts: %d", stateCounts);
	ImGui::SetNextItemWidth(150);

	map<wstring, CState*>::iterator iter = pMap.begin();

	ImGui::Text("State List");
	ImGui::SetNextItemWidth(50);
	int i = 1;
	for (; iter != pMap.end(); ++iter)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
		string ID = std::to_string(i++);
		ImGui::Text(ID.c_str());
		string strState = WStringToString(iter->second->GetName());
		ImGui::SameLine(20);		
		ImGui::Text((char*)strState.c_str());
		ImGui::PopStyleColor();		
	}

	CState* pState = pSM->GetCurState();

	if (nullptr == pState)
	{
		return;
	}

	string strCurState = WStringToString(pState->GetName());

	ImGui::Text("CurState");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150);
	ImGui::InputText("##ActionName", (char*)strCurState.c_str(), strCurState.length(),
		ImGuiInputTextFlags_ReadOnly);



}
