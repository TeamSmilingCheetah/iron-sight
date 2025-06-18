#include "pch.h"
#include "Client/UI/Public/TargetOBUI.h"

#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Component/Base/components.h"

#include "Client/System/Public/CImGuiMgr.h"
#include "Client/UI/Public/Editor/Inspector.h"
#include "Client/Core/Public/CEditorMgr.h"
#include "Client/Script/Public/CGameObjectEx.h"
#include "Client/UI/Public/Editor/Outliner.h"

TargetOBUI::TargetOBUI()
	: EditorUI("TargetObject")
	, m_TargetID(0)
	, m_ParentClick(false)
	, m_PostObject(nullptr)

{
}

TargetOBUI::~TargetOBUI()
{
}

void TargetOBUI::Init()
{
	// 오브젝트 ID가 담긴 텍스쳐 가져오기(Data에 담음)
	m_IDTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DataTargetTex");
	// 위치 텍스쳐
	m_PositionTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"PositionTargetTex");

	m_PostObject = new CGameObjectEx;
	m_PostObject->SetName(L"TargetPost");
	m_PostObject->AddComponent(new CMeshRender);

	// 전용 재질 사용(ID값이랑 텍스쳐를 넣어서 PostProcess로 랜더링하며 검사함)
	m_PostObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	m_PostObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"TargetPostProcessMtrl"), 0);

	m_PostObject->GetRenderComponent()->GetMaterial(0)->SetTexParam(TEX_0, m_IDTex);
	m_PostObject->GetRenderComponent()->GetMaterial(0)->SetTexParam(TEX_1, m_PositionTex);

	CEditorMgr::GetInst()->CreateEditorObj(m_PostObject);

}

void TargetOBUI::Render_Update()
{
	ImGui::Text("MouseActive");
	ImGui::SameLine(100);
	ImGui::Text("Press LCTRL + LBTN");

	// 마우스 체크
	if (KEY_PRESSED(KEY::LCTRL) && KEY_TAP(KEY::LBTN))
	{
		// 현재 마우스 위치 가져오기
		Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

		// IDTexture의 크기 가져오기
		UINT width = m_IDTex->GetWidth();
		UINT height = m_IDTex->GetHeight();

		// 마우스 클릭 발생 시 해당 위치의 IDTex확인
		vector<Vec4> pixels;
		if (m_IDTex->CaptureTextureCustom(pixels))
		{
			// 마우스 위치를 텍스처 좌표로 변환
			int texX = (int)vMousePos.x;
			int texY = (int)vMousePos.y;

			// 범위 체크
			if (texX >= 0 && texX < (int)width && texY >= 0 && texY < (int)height)
			{
				// 해당 위치의 픽셀 인덱스 계산
				int pixelIndex = texY * width + texX;

				// 해당 위치의 픽셀값(ID) 가져오기
				if (pixelIndex < pixels.size())
				{
					Vec4 Data = pixels[pixelIndex];
					UINT parentID = (UINT)Data.x;   // 부모 ID
					UINT childID = (UINT)Data.y;    // 자식 ID

					// 클릭한 위치에 오브젝트가 있는 경우
					if (childID != 0)
					{
						UINT newTargetID = 0;
						bool newParentClick = false;

						// 클릭된 자식오브젝트, 부모 클릭상태아님-> 그대로 유지
						if (m_TargetID == childID && !m_ParentClick)
						{
							newTargetID = childID;
							newParentClick = false;
						}
						// 부모와 현재 타겟이 같음, 부모선택 상태-> 자식으로 접근
						else if (m_TargetID == parentID && m_ParentClick)
						{
							newTargetID = childID;
							newParentClick = false;
						}
						// 같은 부모, 다른 자식, 자식 클릭상태 -> 해당 자식으로 이동
						else if (!m_ParentClick && m_TargetID != childID)
						{
							// 먼저 현재 선택된 자식의 부모 ID를 확인
							CGameObject* currentObject = CLevelMgr::GetInst()->FindObjectByID(m_TargetID);
							if (currentObject && currentObject->GetParent() &&
								currentObject->GetParentObjectID() == parentID)
							{
								newTargetID = childID;
								newParentClick = false;
							}
							// 자식의 부모가 다름 -> 해당 부모로 접근
							else
							{
								newTargetID = parentID;
								newParentClick = true;
							}
						}
						// 기타 경우 -> 부모 ID로 선택
						else
						{
							newTargetID = parentID;
							newParentClick = true;
						}

						m_TargetID = newTargetID;
						m_ParentClick = newParentClick;

						// 타겟 값이 0이 아닌경우에만 inspector에 검색하여 타겟오브젝트 설정
						if (m_TargetID != 0)
						{
							// ID로 오브젝트 검색
							CGameObject* FindObject = CLevelMgr::GetInst()->FindObjectByID(m_TargetID);

							Inspector* pInspector = (Inspector*)CImGuiMgr::GetInst()->FindUI("Inspector");
							pInspector->SetTargetObject(FindObject);


							// Outliner에서 해당 오브젝트 선택 및 스크롤
							Outliner* pOutliner = (Outliner*)CImGuiMgr::GetInst()->FindUI("Outliner");
							if (pOutliner && FindObject)
							{
								pOutliner->SelectAndScrollToObject(FindObject);
							}
						}
					}
				}
			}
		}
	}

	// PostProcess를 담당하는 Object에 추적할 ID값을 보내준다.
	m_PostObject->GetRenderComponent()->GetMaterial(0)->SetScalarParam(INT_0, m_TargetID);
	m_PostObject->GetRenderComponent()->GetMaterial(0)->SetScalarParam(INT_1, (int)m_ParentClick);




	ImGui::Text("Current ID: %u", m_TargetID);
	if (m_TargetID != 0)
	{
		ImGui::Text("Current Object Name: ");
		CGameObject* FindObject = CLevelMgr::GetInst()->FindObjectByID(m_TargetID);

		if (nullptr != FindObject && !(FindObject->IsDead()))
		{
			auto strKey = WStringToString(FindObject->GetName());
			ImGui::InputText("##TexName", (char*)strKey.c_str(), strKey.length(), ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			// 삭제 예정인 오브젝트이거나 없을 경우
			m_TargetID = 0;
			m_ParentClick = false;
		}
	}

	// 초기화 버튼
	if (ImGui::Button("Target Reset"))
	{
		m_TargetID = 0;
		m_ParentClick = false;
	}

}

void TargetOBUI::TraceTargetObject(CGameObject* _Object)
{
	m_TargetID = _Object->GetObjectID();

	// 부모 여부 판단
	CGameObject* parent = _Object->GetParent();
	if (parent)
	{
		// 자식 오브젝트인 경우
		m_ParentClick = false;
	}
	else
	{
		// 부모가 없다는건 최상위 오브젝트
		m_ParentClick = true;
	}
}

void TargetOBUI::Deactivate()
{
	m_PostObject->GetRenderComponent()->GetMaterial(0)->SetScalarParam(INT_0, 0);
	m_PostObject->GetRenderComponent()->GetMaterial(0)->SetScalarParam(INT_1, 0);
}
