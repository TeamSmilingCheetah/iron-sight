#include "pch.h"

#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CRenderMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Client/Core/Public/CEditorMgr.h"
#include "Client/Script/Public/CEditorCamScript.h"
#include "Client/Script/Public/CEditorSpaceCamScript.h"
#include "Client/Script/Public/CGameObjectEx.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"

#include "Engine/Runtime/Public/Component/Light/CLight3D.h"

#include "Engine/System/Public/Rendering/RenderTarget/CMRT.h"

CEditorMgr::CEditorMgr()
	: m_RenderEditorSpace(false)
	, m_EditorSpaceCam(nullptr)
	, m_Light(nullptr)
	, m_Origin(nullptr)
	, m_EditorSpaceRT(nullptr)
	, m_EditorSpaceCamScript(nullptr)
{
}

CEditorMgr::~CEditorMgr()
{
	DeleteVec(m_vecEditorObj);
	DeleteVec(m_vecEditorSpaceObj);

	delete m_EditorSpaceRT;
}

void CEditorMgr::Init()
{
	// =============
	// Editor Camera
	// =============

	CGameObjectEx* pObject = new CGameObjectEx;
	pObject->SetName(L"EditorCamera");
	pObject->AddComponent(new CCamera);
	pObject->AddComponent(new CEditorCamScript);

	pObject->Transform()->SetRelativePos(0.f, 0.f, -200.f);
	pObject->Camera()->LayerCheckAll();
	pObject->Camera()->SetProjType(PERSPECTIVE);
	pObject->Camera()->SetFar(100000.f);

	m_vecEditorObj.push_back(pObject);

	// Editor 용 카메라 등록
	CRenderMgr::GetInst()->RegisterEditorCamera(pObject->Camera());

	// ===================
	// Editor Render Space
	// ===================

	// Render Target
	m_EditorSpaceRT = new CMRT;
	Ptr<CTexture> pRT = CAssetMgr::GetInst()->FindAsset<CTexture>(L"EditorRenderTargetTex");
	m_EditorSpaceRT->Create(&pRT, 1, nullptr);
	m_EditorSpaceRT->SetClearColor(0, Vec4(0.343f, 0.343f, 0.343f, 1.f));

	// Light
	m_Light = new CGameObjectEx;
	m_Light->SetName(L"Directional Light");
	m_Light->AddComponent(new CLight3D);
	m_Light->Transform()->SetRelativePos(0.f, 100.f, 0.f);
	m_Light->Transform()->SetRelativeRotation(45.f, 45.f, 0.f);
	m_Light->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	m_Light->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	m_Light->Light3D()->SetAmbient(Vec3(0.15f, 0.15f, 0.15f));
	m_Light->Light3D()->SetSpecularCoefficient(0.3f);
	m_Light->Light3D()->SetRadius(300.f);

	m_vecEditorSpaceObj.push_back(m_Light);

	// Camera
	m_EditorSpaceCam = new CGameObjectEx;
	m_EditorSpaceCam->SetName(L"EditorSpaceCamera");
	m_EditorSpaceCam->AddComponent(new CCamera);
	m_EditorSpaceCamScript = new CEditorSpaceCamScript;
	m_EditorSpaceCam->AddComponent(m_EditorSpaceCamScript);

	m_EditorSpaceCam->Transform()->SetRelativePos(0.f, 150.f, -150.f);
	m_EditorSpaceCam->Transform()->SetRelativeRotation(30.f, 0.f, 0.f);
	m_EditorSpaceCam->Camera()->SetProjType(PERSPECTIVE);
	m_EditorSpaceCam->Camera()->SetFar(100000.f);

	m_vecEditorSpaceObj.push_back(m_EditorSpaceCam);


	// Origin (원점에 구 하나 표시)
	m_Origin = new CGameObjectEx;
	m_Origin->SetName(L"OriginSphere");
	m_Origin->AddComponent(new CMeshRender);
	m_Origin->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
	m_Origin->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	m_Origin->Transform()->SetRelativePos(0.f, 0.f, 0.f);
	m_Origin->Transform()->SetRelativeScale(10.f, 10.f, 10.f);

	m_vecEditorSpaceObj.push_back(m_Origin);
}

void CEditorMgr::Progress()
{
	if (CLevelMgr::GetInst()->GetCurrentLevel()->GetState() != LEVEL_STATE::STOP)
		return;

	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->Tick();
	}

	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->FinalTick_Editor();
	}

	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->Render_Editor();
	}

	for (int i = 0; i < 8; ++i)
	{
		CTexture::Clear(i);
	}

	if (m_RenderEditorSpace)
	{
		// Tick
		for (size_t i = 0; i < m_vecEditorSpaceObj.size(); ++i)
		{
			m_vecEditorSpaceObj[i]->Tick();
		}

		// Final Tick
		for (size_t i = 0; i < m_vecEditorSpaceObj.size(); ++i)
		{
			m_vecEditorSpaceObj[i]->FinalTick_Editor();
		}

		// Render
		Render_Init();
		Render_Deferred();
		Render_Light();
		Render_Merge();
		Render_Clear();
	}
}

void CEditorMgr::Render_Init()
{
	// Camera View/Proj setting
	g_Trans.matView = m_EditorSpaceCam->Camera()->GetViewMat();
	g_Trans.matProj = m_EditorSpaceCam->Camera()->GetProjMat();
}

void CEditorMgr::Render_Deferred()
{
	CMRT* pDeferredMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED);
	pDeferredMRT->Clear();
	pDeferredMRT->OMSet();

	for (size_t i = 0; i < m_vecEditorSpaceObj.size(); ++i)
	{
		m_vecEditorSpaceObj[i]->Render_Editor();
	}
}

void CEditorMgr::Render_Light()
{
	CMRT* pLightMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT);
	pLightMRT->Clear();
	pLightMRT->OMSet();

	m_Light->Light3D()->Render();
}

void CEditorMgr::Render_Merge()
{
	m_EditorSpaceRT->Clear();
	m_EditorSpaceRT->OMSet();

	Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
	Ptr<CMaterial> pMergeMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"MergeMtrl");
	pMergeMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"ColorTargetTex"));
	pMergeMtrl->SetTexParam(TEX_1, CAssetMgr::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
	pMergeMtrl->SetTexParam(TEX_2, CAssetMgr::GetInst()->FindAsset<CTexture>(L"DiffuseTargetTex"));
	pMergeMtrl->SetTexParam(TEX_3, CAssetMgr::GetInst()->FindAsset<CTexture>(L"SpecularTargetTex"));
	pMergeMtrl->SetTexParam(TEX_4, CAssetMgr::GetInst()->FindAsset<CTexture>(L"EmissiveTargetTex"));
	pMergeMtrl->SetScalarParam(INT_0, 0);
	pMergeMtrl->Binding();

	pRectMesh->Render(0);
}

void CEditorMgr::Render_Clear()
{
	for (int i = 0; i < 8; ++i)
	{
		CTexture::Clear(i);
	}

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
}

void CEditorMgr::CreateEditorObj(CGameObjectEx* _EditorObj)
{
	m_vecEditorObj.push_back(_EditorObj);
}

void CEditorMgr::CreateEditorSpaceObj(CGameObjectEx* _EditorSpaceObj)
{
	m_vecEditorSpaceObj.push_back(_EditorSpaceObj);
}
