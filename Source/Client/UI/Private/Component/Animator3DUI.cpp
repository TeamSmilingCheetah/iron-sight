#include "pch.h"
#include "Client/UI/Public/Component/Animator3DUI.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"

class CAnimator3D;

Animator3DUI::Animator3DUI()
	: ComponentUI("Animator3DUI", COMPONENT_TYPE::ANIMATOR3D)
{
}

Animator3DUI::~Animator3DUI()
{
}


void Animator3DUI::Render_Update()
{
	ComponentTitle("Animator3DUI");
	CAnimator3D* pAnim3D = GetTargetObject()->Animator3D();
	//CMesh* pMesh = GetTargetObject()->MeshRender()->GetM

	const vector<Ptr<CAnimation>> vecClip = pAnim3D->GetClips();

	if (vecClip.size() == 0)
	{
		ImGui::Text("No Animation Clip");
	}
	else
	{

		// ClipIdx
		int ClipIdx = pAnim3D->GetCurClipIdx();
		ImGui::Text("Current Clip IDX:");
		ImGui::SameLine();
		if (ImGui::InputInt("##ClipIdx", &ClipIdx))
		{
			if (ClipIdx < 0)
				ClipIdx = 0;
			if (vecClip.size() - 1 < ClipIdx)
				ClipIdx = static_cast<int>(vecClip.size()) - 1;
			pAnim3D->SetCurClip(ClipIdx);
		}


		// CurClipTime
		//double CurClipTime = pAnim3D->GetCurClipTime();
		//ImGui::Text("Current Clip Time:");
		//ImGui::SameLine();
		//ImGui::InputDouble("##ClipTime", &CurClipTime);
		//pAnim3D->SetClipTime(ClipIdx, static_cast<float>(CurClipTime));


		// FrameIdx
		int FrameIdx = pAnim3D->GetCurFrameIdx();
		ImGui::Text("Current Frame IDX:");
		ImGui::SameLine();
		ImGui::InputInt("##FrameIdx", &FrameIdx);

		// Ratio
		float Ratio = pAnim3D->GetRatio();
		ImGui::Text("Ratio");
		ImGui::SameLine();
		ImGui::InputFloat("##Ratio", &Ratio);


		// CurClipName
		wstring ClipstrName = vecClip[ClipIdx]->GetName();

		string ClipName;
		ClipName = WStringToString(ClipstrName);

		ImGui::Text("ClipName");
		ImGui::SameLine(100);
		ImGui::SetNextItemWidth(150);
		ImGui::InputText("##ClipName", (char*)ClipName.c_str(), ClipName.length(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);


		// BoneCount
		int BoneCount = static_cast<int>(vecClip[ClipIdx]->GetBoneCount());
		ImGui::Text("BoneCount:");
		ImGui::SameLine();
		ImGui::InputInt("##BoneCount", &BoneCount);


		// Animation Stop & Play
		static double CurTime = 0.f;
		if (ImGui::Button("Stop Anim"))
		{
			pAnim3D->Pause();
		}
		ImGui::SameLine(150);
		if (ImGui::Button("Play Anim"))
		{
			pAnim3D->Play();
		}
	}

	if (ImGui::Button("DELETE##Animator3D"))
	{
		DeleteComponent(COMPONENT_TYPE::ANIMATOR3D);
	}
}
