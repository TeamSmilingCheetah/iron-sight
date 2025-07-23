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

	const map<wstring, Ptr<CAnimation>>& mapClip = pAnim3D->GetClips();

	if (mapClip.empty())
	{
		ImGui::Text("No Animation Clip");
	}
	else
	{
		static string curClipName;
		static string nextClipName;

		Ptr<CAnimation> pCurClip = pAnim3D->GetCurClip();
		if (pCurClip == nullptr)
			curClipName = "None";
		else
			curClipName = WStringToString(pCurClip->GetKey());

		// CurClip Info
		ImGui::Text("Cur Clip");
		ImGui::SameLine(100);
		ImGui::SetNextItemWidth(140);
		if (ImGui::BeginCombo("##CurClip", curClipName.c_str())) // The drop-down box
		{
			for (auto iter = mapClip.begin(); iter != mapClip.end(); ++iter)
			{
				string name = WStringToString(iter->second->GetKey());
				bool is_selected = (curClipName == name);
				if (ImGui::Selectable(name.c_str(), is_selected))
				{
					if (curClipName != name)
					{
						curClipName = name;

						pAnim3D->SetCurClip(iter->first);
					}
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus(); // Sets keyboard focus
			}

			ImGui::EndCombo();
		}

		int FrameIdx = pAnim3D->GetCurFrameIdx();
		ImGui::SameLine();
		ImGui::Text("Frame:");
		ImGui::SameLine(); ImGui::SetNextItemWidth(100);
		ImGui::InputInt("##CurFrameIdx", &FrameIdx);

		// NextClip Info
		Ptr<CAnimation> pNextClip = pAnim3D->GetNextClip();
		if (pNextClip == nullptr)
			nextClipName = "None";
		else
			nextClipName = WStringToString(pNextClip->GetKey());

		ImGui::Text("Next Clip");
		ImGui::SameLine(100);
		ImGui::SetNextItemWidth(140);
		if (ImGui::BeginCombo("##NextClip", nextClipName.c_str())) // The drop-down box
		{
			for (auto iter = mapClip.begin(); iter != mapClip.end(); ++iter)
			{
				string name = WStringToString(iter->second->GetKey());
				bool is_selected = (nextClipName == name);
				if (ImGui::Selectable(name.c_str(), is_selected))
				{
					if (nextClipName != name)
					{
						nextClipName = name;

						pAnim3D->SetCurClipBlend(iter->first, 1.f);
					}
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus(); // Sets keyboard focus
			}

			ImGui::EndCombo();
		}

		FrameIdx = pAnim3D->GetNextFrameIdx();
		ImGui::SameLine();
		ImGui::Text("Frame:");
		ImGui::SameLine(); ImGui::SetNextItemWidth(100);
		ImGui::InputInt("##NextFrameIdx", &FrameIdx);

		// Animation Stop & Play
		static double CurTime = 0.f;
		if (ImGui::Button("Stop Anim"))
			pAnim3D->Pause();

		ImGui::SameLine();
		if (ImGui::Button("Play Anim"))
			pAnim3D->Play();

		ImGui::SameLine();
		if (ImGui::Button("Add Anim"))
		{
			wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
			strContentPath += L"Animation\\";

			// 파일 경로 문자열
			wchar_t szFilePath[255] = {};

			OPENFILENAME Desc = {};

			Desc.lStructSize = sizeof(OPENFILENAME);
			Desc.hwndOwner = nullptr;
			Desc.lpstrFile = szFilePath;
			Desc.nMaxFile = 255;
			Desc.lpstrFilter = L"Animation\0*.anim\0ALL\0*.*";
			Desc.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			Desc.lpstrInitialDir = strContentPath.c_str();

			if (GetOpenFileName(&Desc))
			{
				wstring strFilePath = szFilePath;
				wstring Content = L"\\Content\\";

				strFilePath = strFilePath.substr(strFilePath.find(Content) + Content.size(), strFilePath.size());

				pAnim3D->AddAnimClip(CAssetMgr::GetInst()->Load<CAnimation>(strFilePath));
			}
		}
	}

	if (ImGui::Button("DELETE##Animator3D"))
	{
		DeleteComponent(COMPONENT_TYPE::ANIMATOR3D);
	}
}
