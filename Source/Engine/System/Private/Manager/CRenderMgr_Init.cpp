#include "pch.h"
#include "System/Public/Asset/Base/assets.h"
#include "System/Public/Manager/CAssetMgr.h"
#include "System/Public/Manager/CRenderMgr.h"
#include "System/Public/Rendering/Device/CDevice.h"
#include "System/Public/Rendering/RenderTarget/CMRT.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "Runtime/Public/Component/Rendering/CMeshRender.h"

void CRenderMgr::Init()
{
	CreateMRT();
	m_arrMRT[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->OMSet();

	CreateDebugMtrl();

	CreateRenderMtrl();

	// DebugRender 용 DummyObject
	m_DbgObj = new CGameObject;
	m_DbgObj->AddComponent(new CMeshRender);

	// PostProcess 용 텍스쳐
	m_PostProcessTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"PostProcessTex");
}

void CRenderMgr::CreateMRT()
{
	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();


	// ============
	// SwapChainMRT
	// ============
	{
		m_arrMRT[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)] = new CMRT;
		m_arrMRT[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->SetName(L"SwapChain");

		Ptr<CTexture> pRTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
		Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");

		m_arrMRT[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->Create(&pRTTex, 1, pDSTex);
		m_arrMRT[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->
			SetClearColor(0, Vec4(0.f, 0.f, 0.f, 1.f));
	}

	// ========
	// Deferred
	// ========
	{
		m_arrMRT[static_cast<UINT>(MRT_TYPE::DEFERRED)] = new CMRT;
		m_arrMRT[static_cast<UINT>(MRT_TYPE::DEFERRED)]->SetName(L"Deferred");

		Ptr<CTexture> arrTex[8] =
		{
			CAssetMgr::GetInst()->CreateTexture(L"ColorTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R8G8B8A8_UNORM,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"NormalTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32G32B32A32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"PositionTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32G32B32A32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"EmissiveTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R8G8B8A8_UNORM,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"DataTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32G32B32A32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
		};

		Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");

		m_arrMRT[static_cast<UINT>(MRT_TYPE::DEFERRED)]->Create(arrTex, 5, pDSTex);
		m_arrMRT[static_cast<UINT>(MRT_TYPE::DEFERRED)]->SetClearColor(
			0, Vec4(0.f, 0.f, 0.f, 0.f), false);
	}

	// =====
	// Decal
	// =====
	{
		m_arrMRT[static_cast<UINT>(MRT_TYPE::DECAL)] = new CMRT;
		m_arrMRT[static_cast<UINT>(MRT_TYPE::DECAL)]->SetName(L"Decal");

		Ptr<CTexture> arrTex[8] =
		{
			CAssetMgr::GetInst()->FindAsset<CTexture>(L"ColorTargetTex"),
			CAssetMgr::GetInst()->FindAsset<CTexture>(L"EmissiveTargetTex"),
		};

		Ptr<CTexture> pDSTex = nullptr;
		m_arrMRT[static_cast<UINT>(MRT_TYPE::DECAL)]->Create(arrTex, 2, pDSTex);
	}

	// =====
	// LIGHT
	// =====
	{
		m_arrMRT[static_cast<UINT>(MRT_TYPE::LIGHT)] = new CMRT;
		m_arrMRT[static_cast<UINT>(MRT_TYPE::LIGHT)]->SetName(L"Light");

		Ptr<CTexture> arrTex[8] =
		{
			CAssetMgr::GetInst()->CreateTexture(L"DiffuseTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32G32B32A32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"SpecularTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32G32B32A32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
		};

		m_arrMRT[static_cast<UINT>(MRT_TYPE::LIGHT)]->Create(arrTex, 2, nullptr);
		m_arrMRT[static_cast<UINT>(MRT_TYPE::LIGHT)]->SetClearColor(
			0, Vec4(0.f, 0.f, 0.f, 0.f), false);
	}
}

void CRenderMgr::CreateDebugMtrl()
{
	// =================================
	// DebugShapeShader : 디버그용 쉐이더
	// =================================
	Ptr<CGraphicShader> pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\debug.fx", "VS_DebugShape");
	pShader->CreatePixelShader(L"Shader\\debug.fx", "PS_DebugShape");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	CAssetMgr::GetInst()->AddAsset(L"DebugShapeShader", pShader);

	// DebugShapeMtrl
	Ptr<CMaterial> pMtrl = new CMaterial(true);
	pMtrl->SetName(L"DebugShapeMtrl");
	pMtrl->SetShader(pShader);
	CAssetMgr::GetInst()->AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);


	// ================
	// DebugShapeSphere
	// ================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\debug.fx", "VS_DebugShapeSphere");
	pShader->CreatePixelShader(L"Shader\\debug.fx", "PS_DebugShapeSphere");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	CAssetMgr::GetInst()->AddAsset(L"DebugShapeSphereShader", pShader);

	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"DebugShapeSphereMtrl");
	pMtrl->SetShader(pShader);
	CAssetMgr::GetInst()->AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);


	// ==============
	// DebugShapeLine
	// ==============
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\debug.fx", "VS_DebugShapeLine");
	pShader->CreateGeometryShader(L"Shader\\debug.fx", "GS_DebugShapeLine");
	pShader->CreatePixelShader(L"Shader\\debug.fx", "PS_DebugShapeLine");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST); // 입력 메쉬 기준
	CAssetMgr::GetInst()->AddAsset(L"DebugShapeLineShader", pShader);

	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"DebugShapeLineMtrl");
	pMtrl->SetShader(pShader);
	CAssetMgr::GetInst()->AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// ==============
	// Skeleton Debug
	// ==============
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\debug.fx", "VS_DebugSkeleton");
	pShader->CreateGeometryShader(L"Shader\\debug.fx", "GS_DebugSkeleton");
	pShader->CreatePixelShader(L"Shader\\debug.fx", "PS_DebugSkeleton");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	CAssetMgr::GetInst()->AddAsset(L"DebugSkeletonShader", pShader);

	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"DebugSkeletonMtrl");
	pMtrl->SetShader(pShader);
	CAssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl);
}

void CRenderMgr::CreateRenderMtrl()
{
	// ===========
	// MergeShader
	// ===========
	Ptr<CGraphicShader> pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\merge.fx", "VS_Merge");
	pShader->CreatePixelShader(L"Shader\\merge.fx", "PS_Merge");
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	CAssetMgr::GetInst()->AddAsset(L"MergeShader", pShader);

	m_MergeMtrl = new CMaterial(true);
	m_MergeMtrl->SetName(L"MergeMtrl");
	m_MergeMtrl->SetShader(pShader);
	CAssetMgr::GetInst()->AddAsset<CMaterial>(m_MergeMtrl->GetName(), m_MergeMtrl);


	// ==============
	// DirLightShader
	// ==============
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_DirLight");
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_DirLight");
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSState(BS_TYPE::ONE_ONE);
	CAssetMgr::GetInst()->AddAsset(L"DirLightShader", pShader);

	Ptr<CMaterial> pLightMtrl = new CMaterial(true);
	pLightMtrl->SetName(L"DirLightMtrl");
	pLightMtrl->SetShader(pShader);
	pLightMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
	pLightMtrl->SetTexParam(TEX_1, CAssetMgr::GetInst()->FindAsset<CTexture>(L"NormalTargetTex"));
	CAssetMgr::GetInst()->AddAsset<CMaterial>(pLightMtrl->GetName(), pLightMtrl);


	// ================
	// PointLightShader
	// ================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_PointLight");
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_PointLight");
	pShader->SetRSState(RS_TYPE::CULL_FRONT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSState(BS_TYPE::ONE_ONE);
	CAssetMgr::GetInst()->AddAsset(L"PointLightShader", pShader);

	pLightMtrl = new CMaterial(true);
	pLightMtrl->SetName(L"PointLightMtrl");
	pLightMtrl->SetShader(pShader);
	pLightMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
	pLightMtrl->SetTexParam(TEX_1, CAssetMgr::GetInst()->FindAsset<CTexture>(L"NormalTargetTex"));
	CAssetMgr::GetInst()->AddAsset<CMaterial>(pLightMtrl->GetName(), pLightMtrl);
}

CCamera* CRenderMgr::GetMainCamera()
{
	if (m_IsEditor)
		return m_EditorCam;
	if (!m_vecCam.empty())
		return m_vecCam[0];
	return nullptr;
}
