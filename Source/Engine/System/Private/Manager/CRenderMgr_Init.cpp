#include "pch.h"
#include "Engine/System/Public/Manager/CRenderMgr.h"

#include "Engine/System/Public/Manager/AssetManager.h"
#include "Engine/System/Public/Rendering/Device/CDevice.h"
#include "Engine/System/Public/Rendering/RenderTarget/CMRT.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"

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
	m_PostProcessTex = FAssetManager::GetInst()->FindAsset<CTexture>(L"PostProcessTex");
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

		Ptr<CTexture> pRTTex = FAssetManager::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
		Ptr<CTexture> pDSTex = FAssetManager::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");

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
			FAssetManager::GetInst()->CreateTexture(L"ColorTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R8G8B8A8_UNORM,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			FAssetManager::GetInst()->CreateTexture(L"NormalTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32G32B32A32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			FAssetManager::GetInst()->CreateTexture(L"PositionTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32G32B32A32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			FAssetManager::GetInst()->CreateTexture(L"MetallicTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			FAssetManager::GetInst()->CreateTexture(L"RoughnessTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			FAssetManager::GetInst()->CreateTexture(L"AOTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			FAssetManager::GetInst()->CreateTexture(L"EmissiveTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R8G8B8A8_UNORM,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			FAssetManager::GetInst()->CreateTexture(L"DataTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32G32B32A32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
		};

		Ptr<CTexture> pDSTex = FAssetManager::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");

		m_arrMRT[static_cast<UINT>(MRT_TYPE::DEFERRED)]->Create(arrTex, 8, pDSTex);
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
			FAssetManager::GetInst()->FindAsset<CTexture>(L"ColorTargetTex"),
			FAssetManager::GetInst()->FindAsset<CTexture>(L"EmissiveTargetTex"),
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
			FAssetManager::GetInst()->CreateTexture(L"DiffuseTargetTex"
												, static_cast<UINT>(vResolution.x),
												static_cast<UINT>(vResolution.y)
												, DXGI_FORMAT_R32G32B32A32_FLOAT,
												D3D11_BIND_RENDER_TARGET |
												D3D11_BIND_SHADER_RESOURCE),
			FAssetManager::GetInst()->CreateTexture(L"SpecularTargetTex"
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

	// ========
	// Minimap
	// ========
	{
		m_arrMRT[static_cast<UINT>(MRT_TYPE::MINIMAP)] = new CMRT;
		m_arrMRT[static_cast<UINT>(MRT_TYPE::MINIMAP)]->SetName(L"Minimap");

		Ptr<CTexture> pMinimapTex = FAssetManager::GetInst()->CreateTexture(
			L"MinimapTargetTex", 256, 256,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

		Ptr<CTexture> pMinimapDepth = FAssetManager::GetInst()->CreateTexture(
			L"MinimapDepthTex", 256, 256,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			D3D11_BIND_DEPTH_STENCIL);

		m_arrMRT[static_cast<UINT>(MRT_TYPE::MINIMAP)]->Create(&pMinimapTex, 1, pMinimapDepth);
		m_arrMRT[static_cast<UINT>(MRT_TYPE::MINIMAP)]->SetClearColor(0, Vec4(0.f, 0.f, 0.f, 0.f));
	}
}

void CRenderMgr::CreateDebugMtrl()
{
	// =================================
	// DebugShapeShader : 디버그용 쉐이더
	// =================================
	Ptr<CGraphicShader> pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"debug_shape_vs.cso", L"debug.fx", L"VS_DebugShape");
	pShader->CreatePixelShader(L"debug_shape_ps.cso", L"debug.fx", L"PS_DebugShape");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	FAssetManager::GetInst()->AddAsset(L"DebugShapeShader", pShader);

	// DebugShapeMtrl
	Ptr<CMaterial> pMtrl = new CMaterial(true);
	pMtrl->SetName(L"DebugShapeMtrl");
	pMtrl->SetShader(pShader);
	FAssetManager::GetInst()->AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);


	// ================
	// DebugShapeSphere
	// ================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"debug_shape_sphere_vs.cso", L"debug.fx", L"VS_DebugShapeSphere");
	pShader->CreatePixelShader(L"debug_shape_sphere_ps.cso", L"debug.fx", L"PS_DebugShapeSphere");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	FAssetManager::GetInst()->AddAsset(L"DebugShapeSphereShader", pShader);

	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"DebugShapeSphereMtrl");
	pMtrl->SetShader(pShader);
	FAssetManager::GetInst()->AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);


	// ==============
	// DebugShapeLine
	// ==============
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"debug_shape_line_vs.cso", L"debug.fx", L"VS_DebugShapeLine");
	pShader->CreateGeometryShader(L"debug_shape_line_gs.cso", L"debug.fx", L"GS_DebugShapeLine");
	pShader->CreatePixelShader(L"debug_shape_line_ps.cso", L"debug.fx", L"PS_DebugShapeLine");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST); // 입력 메쉬 기준
	FAssetManager::GetInst()->AddAsset(L"DebugShapeLineShader", pShader);

	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"DebugShapeLineMtrl");
	pMtrl->SetShader(pShader);
	FAssetManager::GetInst()->AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// ==============
	// Skeleton Debug
	// ==============
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"debug_skeleton_vs.cso", L"debug.fx", L"VS_DebugSkeleton");
	pShader->CreateGeometryShader(L"debug_skeleton_gs.cso", L"debug.fx", L"GS_DebugSkeleton");
	pShader->CreatePixelShader(L"debug_skeleton_ps.cso", L"debug.fx", L"PS_DebugSkeleton");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	FAssetManager::GetInst()->AddAsset(L"DebugSkeletonShader", pShader);

	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"DebugSkeletonMtrl");
	pMtrl->SetShader(pShader);
	FAssetManager::GetInst()->AddAsset(pMtrl->GetName(), pMtrl);
}

void CRenderMgr::CreateRenderMtrl()
{
	// ===========
	// MergeShader
	// ===========
	Ptr<CGraphicShader> pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"merge_vs.cso", L"merge.fx", L"VS_Merge");
	pShader->CreatePixelShader(L"merge_ps.cso", L"merge.fx", L"PS_Merge");
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	FAssetManager::GetInst()->AddAsset(L"MergeShader", pShader);

	m_MergeMtrl = new CMaterial(true);
	m_MergeMtrl->SetName(L"MergeMtrl");
	m_MergeMtrl->SetShader(pShader);
	FAssetManager::GetInst()->AddAsset<CMaterial>(m_MergeMtrl->GetName(), m_MergeMtrl);


	// ==============
	// DirLightShader
	// ==============
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"directional_light_vs.cso", L"light.fx", L"VS_DirLight");
	pShader->CreatePixelShader(L"directional_light_ps.cso", L"light.fx", L"PS_DirLight");
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSState(BS_TYPE::ONE_ONE);
	FAssetManager::GetInst()->AddAsset(L"DirLightShader", pShader);

	Ptr<CMaterial> pLightMtrl = new CMaterial(true);
	pLightMtrl->SetName(L"DirLightMtrl");
	pLightMtrl->SetShader(pShader);
	pLightMtrl->SetTexParam(TEX_0, FAssetManager::GetInst()->FindAsset<CTexture>(L"ColorTargetTex"));
	pLightMtrl->SetTexParam(TEX_1, FAssetManager::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
	pLightMtrl->SetTexParam(TEX_2, FAssetManager::GetInst()->FindAsset<CTexture>(L"NormalTargetTex"));
	pLightMtrl->SetTexParam(TEX_3, FAssetManager::GetInst()->FindAsset<CTexture>(L"MetallicTargetTex"));
	pLightMtrl->SetTexParam(TEX_4, FAssetManager::GetInst()->FindAsset<CTexture>(L"RoughnessTargetTex"));
	FAssetManager::GetInst()->AddAsset<CMaterial>(pLightMtrl->GetName(), pLightMtrl);


	// ================
	// PointLightShader
	// ================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"point_light_vs.cso", L"light.fx", L"VS_PointLight");
	pShader->CreatePixelShader(L"point_light_ps.cso", L"light.fx", L"PS_PointLight");
	pShader->SetRSState(RS_TYPE::CULL_FRONT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSState(BS_TYPE::ONE_ONE);
	FAssetManager::GetInst()->AddAsset(L"PointLightShader", pShader);

	pLightMtrl = new CMaterial(true);
	pLightMtrl->SetName(L"PointLightMtrl");
	pLightMtrl->SetShader(pShader);
	pLightMtrl->SetTexParam(TEX_0, FAssetManager::GetInst()->FindAsset<CTexture>(L"ColorTargetTex"));
	pLightMtrl->SetTexParam(TEX_1, FAssetManager::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
	pLightMtrl->SetTexParam(TEX_2, FAssetManager::GetInst()->FindAsset<CTexture>(L"NormalTargetTex"));
	pLightMtrl->SetTexParam(TEX_3, FAssetManager::GetInst()->FindAsset<CTexture>(L"MetallicTargetTex"));
	pLightMtrl->SetTexParam(TEX_4, FAssetManager::GetInst()->FindAsset<CTexture>(L"RoughnessTargetTex"));
	FAssetManager::GetInst()->AddAsset<CMaterial>(pLightMtrl->GetName(), pLightMtrl);
}

CCamera* CRenderMgr::GetMainCamera()
{
	if (m_IsEditor)
		return m_EditorCam;
	if (!m_vecCam.empty())
		return m_vecCam[0];
	return nullptr;
}
