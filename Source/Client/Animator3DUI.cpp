#include "pch.h"
#include "Animator3DUI.h"

#include <Engine/CAnimator3D.h>
#include <Engine/CMesh.h>

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

	// ClipIdx
	int ClipIdx = pAnim3D->GetCurClipIdx();
	ImGui::Text("Current Clip IDX:");
	ImGui::SameLine();
	ImGui::InputInt("##ClipIdx", &ClipIdx);

	// CurClipTime
	double CurClipTime = pAnim3D->GetCurClipTime();
	ImGui::Text("Current Clip Time:");
	ImGui::SameLine();
	ImGui::InputDouble("##ClipTime", &CurClipTime);
	pAnim3D->SetClipTime(ClipIdx, static_cast<float>(CurClipTime));


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
}
