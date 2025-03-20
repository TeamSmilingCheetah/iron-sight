#include "pch.h"
#include "Client/UI/Public/Component/CameraUI.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"

class CCamera;

CameraUI::CameraUI()
	: ComponentUI("CameraUI", COMPONENT_TYPE::CAMERA)
{
}

CameraUI::~CameraUI()
{
}

void CameraUI::Render_Update()
{
	ComponentTitle("Camera");

	CCamera* pCamera = GetTargetObject()->Camera();

	ImGui::Text("Projection Type");
	ImGui::SameLine(120);

	// PROJ_TYPE
	const char* szProjType[2] = {"Perspective", "Orthographic"};
	int ProjType = pCamera->GetProjType();
	if (ImGui::Combo("##combo", &ProjType, szProjType, 2))
		pCamera->SetProjType(static_cast<PROJ_TYPE>(ProjType));

	// Far
	ImGui::Text("Far");
	ImGui::SameLine(120);
	float Far = pCamera->GetFar();
	if (ImGui::InputFloat("##Far", &Far))
	{
		// Far 가 Near(1) 보다 작거나 같으면 안된다. 시야범위는 Near 에서 Far 사이이기 때문
		if (Far <= 1.f)
			Far = 1.1f;
		pCamera->SetFar(Far);
	}


	// Width
ImGui::BeginDisabled(ProjType == (int)PROJ_TYPE::PERSPECTIVE);

	float Width = pCamera->GetWidth();
	ImGui::Text("Width");
	ImGui::SameLine(120);
	if (ImGui::InputFloat("##Width", &Width))
		pCamera->SetWidth(Width);

ImGui::EndDisabled();

	float AspectRatio = pCamera->GetAspectRatio();
	ImGui::Text("AspectRatio");
	ImGui::SameLine(120);
	if (ImGui::DragFloat("##AspectRatio", &AspectRatio, 0.01f))
		pCamera->SetAspectRatio(AspectRatio);


	// 현재 카메라 투영 옵션이 직교투영이면 비활성화
ImGui::BeginDisabled(ProjType == static_cast<int>(PROJ_TYPE::ORTHOGRAPHIC));

	// 원근투영 전용
	float FOV = pCamera->GetFOV();
	ImGui::Text("Field Of View");
	ImGui::SameLine(120);
	FOV = (FOV * 180.f) / XM_PI;
	if (ImGui::DragFloat("##FOV", &FOV, 0.1f))
	{
		FOV = (FOV * XM_PI) / 180.f;
		pCamera->SetFOV(FOV);
	}

ImGui::EndDisabled();


	// 현재 카메라 투영 옵션이 직교투영이면 비활성화
ImGui::BeginDisabled(ProjType == static_cast<int>(PROJ_TYPE::PERSPECTIVE));

	// 직교투영 전용 변수
	float Scale = pCamera->GetScale();
	ImGui::Text("Scale");
	ImGui::SameLine(120);
	FOV = (FOV * 180.f) / XM_PI;
	if (ImGui::DragFloat("##Scale", &Scale, 0.1f))
	{
		FOV = (FOV * XM_PI) / 180.f;

		// 직교투영 배율이 0 이하로 내려가면 안된다.
		if (Scale <= 0.f)
			Scale = 0.1f;

		pCamera->SetScale(Scale);
	}

ImGui::EndDisabled();

	// 공통
	// UINT m_LayerCheck; // 카메라가 찍을 레이어 비트 체크
	// int m_Priority; // 카메라 우선순위, 0 : MainCamera, -1 : 미등록

	// LayerCheck
	UINT layerCheck = pCamera->GetLayerCheck();

	CLevel* pLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	static string LayerName[MAX_LAYER]{};
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		LayerName[i] = WStringToString(pLevel->GetLayer(i)->GetName());
	}

	if (ImGui::TreeNode("LayerCheck"))
	{
		for (int i = 0; i < MAX_LAYER; ++i)
		{
			if (LayerName[i] == "") continue;

			// 변화가 생기면 LayerCheck(토글)
			if (ImGui::Selectable(LayerName[i].c_str(), layerCheck & (1 << i)))
			{
				pCamera->LayerCheck(i);
			}
		}
		ImGui::TreePop();
	}


	if (ImGui::Button("DELETE##Camera"))
	{
		DeleteComponent(COMPONENT_TYPE::CAMERA);
	}
}
