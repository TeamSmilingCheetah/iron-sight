#include "pch.h"
#include "Client/UI/Public/Asset/AnimationUI.h"
#include "Client/UI/Public/Editor/ParamUI.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Client/Core/Public/CEditorMgr.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"

#include "Client/Script/Public/CEditorSpaceCamScript.h"

AnimationUI::AnimationUI()
	: AssetUI("Animation", ANIMATION)
{
}

AnimationUI::~AnimationUI()
{
}

void AnimationUI::Render_Update()
{
	AssetTitle();

	if (!m_SkinnedModel)
	{
		m_SkinnedModel = CAssetMgr::GetInst()->LoadFBX(L"FBX\\Character\\James.fbx")->Instantiate();
		CEditorMgr::GetInst()->SetEditorSpaceRender(true);
		CEditorMgr::GetInst()->CreateEditorSpaceObj((CGameObjectEx*)m_SkinnedModel);

		if (!m_SkinnedModel->Animator3D())
			m_SkinnedModel->AddComponent(new CAnimator3D);
	}

	if (IsDirty())
	{
		Ptr<CAnimation> pAnim = static_cast<CAnimation*>(GetAsset().Get());

		m_SkinnedModel->Animator3D()->ClearAnimClip();
		m_SkinnedModel->Animator3D()->AddAnimClip(pAnim);
		m_SkinnedModel->Animator3D()->SetCurClipFrame(0);
		m_FrameRange[0] = 0;
		m_FrameRange[1] = m_SkinnedModel->Animator3D()->GetCurClip()->GetFrameLength() - 1;
	}

	Ptr<CTexture> pAnimRT = CAssetMgr::GetInst()->FindAsset<CTexture>(L"EditorRenderTargetTex");

	float windowWidth = ImGui::GetContentRegionAvail().x;

	static const float aspectRatio = 192.f / 340.f;
	static const float imageHeight = windowWidth * aspectRatio;

	ImVec2 uv_min = ImVec2(0.0f, 0.0f);
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

	//ImGui::Image(pAnimRT->GetSRV().Get(), ImVec2(windowWidth, imageHeight), uv_min, uv_max, tint_col, border_col);

	ImGui::ImageButton(pAnimRT->GetSRV().Get(), ImVec2(windowWidth, imageHeight), uv_min, uv_max, 0);

	if (ImGui::IsItemHovered())
	{
		CEditorSpaceCamScript* pCamScript = CEditorMgr::GetInst()->GetEditorSpaceCamScript();
		
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
			pCamScript->SetMouseButton(ImGuiMouseButton_Left);
			pCamScript->SetMouseDelta(MouseDelta.x, MouseDelta.y);

			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
		}
		else if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
		{
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
			pCamScript->SetMouseButton(ImGuiMouseButton_Right);
			pCamScript->SetMouseDelta(MouseDelta.x, MouseDelta.y);

			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
		}
		else if (ImGui::GetIO().MouseWheel)
		{
			pCamScript->SetMouseButton(ImGuiMouseButton_Middle);
			pCamScript->SetMouseDelta(0.f, ImGui::GetIO().MouseWheel);
		}
	}

	int FrameIdx = m_SkinnedModel->Animator3D()->GetCurFrameIdx();
	int FrameLength = m_SkinnedModel->Animator3D()->GetCurClip()->GetFrameLength();

	if (m_SkinnedModel->Animator3D()->IsActive())
	{
		if (ImGui::Button("Pause##Animation", ImVec2(50.f, 18.f)))
		{
			m_SkinnedModel->Animator3D()->Pause();
		}
	}
	else
	{
		if (ImGui::Button("Play##Animation", ImVec2(50.f, 18.f)))
		{
			m_SkinnedModel->Animator3D()->Play();
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("<##Animation", ImVec2(18.f, 18.f)))
	{
		m_SkinnedModel->Animator3D()->SetCurClipFrame(FrameIdx-1);
	}
	
	ImGui::SameLine();

	if (ImGui::SliderInt("##AnimationFrameControl", &FrameIdx, 0, FrameLength-1))
	{
		m_SkinnedModel->Animator3D()->SetCurClipFrame(FrameIdx);
	}

	ImGui::SameLine();

	if (ImGui::Button(">##Animation", ImVec2(18.f, 18.f)))
	{
		m_SkinnedModel->Animator3D()->SetCurClipFrame(FrameIdx + 1);
	}

	// Crop
	if (ImGui::Button("Crop##AnimationCrop", ImVec2(50.f, 18.f)))
	{
		m_SkinnedModel->Animator3D()->Crop(m_FrameRange[0], m_FrameRange[1]);
	}

	ImGui::SameLine();
	ImGui::Text("Begin");

	ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputInt("##AnimationCropFrame1", &m_FrameRange[0]);

	ImGui::SameLine();
	ImGui::Text("End");

	ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputInt("##AnimationCropFrame2", &m_FrameRange[1]);


	SaveButton();
}
